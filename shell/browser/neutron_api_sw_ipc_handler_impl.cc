// Copyright (c) 2025 Salesforce, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "shell/browser/neutron_api_sw_ipc_handler_impl.h"

#include <utility>

#include "base/containers/unique_ptr_adapters.h"
#include "base/notimplemented.h"
#include "content/public/browser/render_frame_host.h"
#include "content/public/browser/render_process_host.h"
#include "mojo/public/cpp/bindings/self_owned_receiver.h"
#include "shell/browser/api/neutron_api_session.h"
#include "shell/browser/neutron_browser_context.h"
#include "shell/browser/javascript_environment.h"
#include "shell/common/gin_helper/dictionary.h"

namespace neutron {

namespace {
const void* const kUserDataKey = &kUserDataKey;

class ServiceWorkerIPCList : public base::SupportsUserData::Data {
 public:
  std::vector<std::unique_ptr<NeutronApiSWIPCHandlerImpl>> list;

  static ServiceWorkerIPCList* Get(
      content::RenderProcessHost* render_process_host,
      bool create_if_not_exists) {
    auto* service_worker_ipc_list = static_cast<ServiceWorkerIPCList*>(
        render_process_host->GetUserData(kUserDataKey));
    if (!service_worker_ipc_list && !create_if_not_exists) {
      return nullptr;
    }
    if (!service_worker_ipc_list) {
      auto new_ipc_list = std::make_unique<ServiceWorkerIPCList>();
      service_worker_ipc_list = new_ipc_list.get();
      render_process_host->SetUserData(kUserDataKey, std::move(new_ipc_list));
    }
    return service_worker_ipc_list;
  }
};

}  // namespace

NeutronApiSWIPCHandlerImpl::NeutronApiSWIPCHandlerImpl(
    content::RenderProcessHost* render_process_host,
    int64_t version_id,
    mojo::PendingAssociatedReceiver<mojom::NeutronApiIPC> receiver)
    : render_process_host_(render_process_host), version_id_(version_id) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);

  receiver_.Bind(std::move(receiver));
  receiver_.set_disconnect_handler(
      base::BindOnce(&NeutronApiSWIPCHandlerImpl::RemoteDisconnected,
                     base::Unretained(this)));

  render_process_host_->AddObserver(this);
}

NeutronApiSWIPCHandlerImpl::~NeutronApiSWIPCHandlerImpl() {
  render_process_host_->RemoveObserver(this);
}

void NeutronApiSWIPCHandlerImpl::RemoteDisconnected() {
  receiver_.reset();
  Destroy();
}

void NeutronApiSWIPCHandlerImpl::Message(bool internal,
                                          const std::string& channel,
                                          blink::CloneableMessage arguments) {
  gin::WeakCell<api::Session>* session = GetSession();
  if (session && session->Get()) {
    v8::Isolate* isolate = neutron::JavascriptEnvironment::GetIsolate();
    v8::HandleScope handle_scope(isolate);
    auto* event = MakeIPCEvent(isolate, session->Get(), internal);
    if (!event)
      return;
    v8::Local<v8::Object> event_object =
        event->GetWrapper(isolate).ToLocalChecked();
    session->Get()->Message(event_object, channel, std::move(arguments));
  }
}

void NeutronApiSWIPCHandlerImpl::Invoke(bool internal,
                                         const std::string& channel,
                                         blink::CloneableMessage arguments,
                                         InvokeCallback callback) {
  gin::WeakCell<api::Session>* session = GetSession();
  if (session && session->Get()) {
    v8::Isolate* isolate = neutron::JavascriptEnvironment::GetIsolate();
    v8::HandleScope handle_scope(isolate);
    auto* event =
        MakeIPCEvent(isolate, session->Get(), internal, std::move(callback));
    if (!event)
      return;
    v8::Local<v8::Object> event_object =
        event->GetWrapper(isolate).ToLocalChecked();
    session->Get()->Invoke(event_object, channel, std::move(arguments));
  }
}

void NeutronApiSWIPCHandlerImpl::ReceivePostMessage(
    const std::string& channel,
    blink::TransferableMessage message) {
  gin::WeakCell<api::Session>* session = GetSession();
  if (session && session->Get()) {
    v8::Isolate* isolate = neutron::JavascriptEnvironment::GetIsolate();
    v8::HandleScope handle_scope(isolate);
    auto* event = MakeIPCEvent(isolate, session->Get(), false);
    if (!event)
      return;
    v8::Local<v8::Object> event_object =
        event->GetWrapper(isolate).ToLocalChecked();
    session->Get()->ReceivePostMessage(event_object, channel,
                                       std::move(message));
  }
}

void NeutronApiSWIPCHandlerImpl::MessageSync(bool internal,
                                              const std::string& channel,
                                              blink::CloneableMessage arguments,
                                              MessageSyncCallback callback) {
  gin::WeakCell<api::Session>* session = GetSession();
  if (session && session->Get()) {
    v8::Isolate* isolate = neutron::JavascriptEnvironment::GetIsolate();
    v8::HandleScope handle_scope(isolate);
    auto* event =
        MakeIPCEvent(isolate, session->Get(), internal, std::move(callback));
    if (!event)
      return;
    v8::Local<v8::Object> event_object =
        event->GetWrapper(isolate).ToLocalChecked();
    session->Get()->MessageSync(event_object, channel, std::move(arguments));
  }
}

void NeutronApiSWIPCHandlerImpl::MessageHost(
    const std::string& channel,
    blink::CloneableMessage arguments) {
  NOTIMPLEMENTED();  // Service workers have no <webview>
}

NeutronBrowserContext* NeutronApiSWIPCHandlerImpl::GetBrowserContext() {
  auto* browser_context = static_cast<NeutronBrowserContext*>(
      render_process_host_->GetBrowserContext());
  return browser_context;
}

gin::WeakCell<api::Session>* NeutronApiSWIPCHandlerImpl::GetSession() {
  return api::Session::FromBrowserContext(GetBrowserContext());
}

gin_helper::internal::Event* NeutronApiSWIPCHandlerImpl::MakeIPCEvent(
    v8::Isolate* isolate,
    api::Session* session,
    bool internal,
    neutron::mojom::NeutronApiIPC::InvokeCallback callback) {
  if (!session) {
    if (callback) {
      // We must always invoke the callback if present.
      gin_helper::internal::ReplyChannel::Create(isolate, std::move(callback))
          ->SendError("Session does not exist");
    }
    return {};
  }

  gin_helper::internal::Event* event =
      gin_helper::internal::Event::New(isolate);
  v8::Local<v8::Object> event_object =
      event->GetWrapper(isolate).ToLocalChecked();

  gin_helper::Dictionary dict(isolate, event_object);
  dict.Set("type", "service-worker");
  dict.Set("versionId", version_id_);
  dict.Set("processId", render_process_host_->GetID().GetUnsafeValue());

  // Set session to provide context for getting preloads
  dict.Set("session", session);

  if (callback)
    dict.Set("_replyChannel", gin_helper::internal::ReplyChannel::Create(
                                  isolate, std::move(callback)));

  if (internal)
    dict.SetHidden("internal", internal);

  return event;
}

void NeutronApiSWIPCHandlerImpl::Destroy() {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);

  auto* service_worker_ipc_list = ServiceWorkerIPCList::Get(
      render_process_host_, /*create_if_not_exists=*/false);
  CHECK(service_worker_ipc_list);
  // std::erase_if will lead to a call to the destructor for this object.
  std::erase_if(service_worker_ipc_list->list, base::MatchesUniquePtr(this));
}

void NeutronApiSWIPCHandlerImpl::RenderProcessExited(
    content::RenderProcessHost* host,
    const content::ChildProcessTerminationInfo& info) {
  CHECK_EQ(host, render_process_host_);
  // TODO(crbug.com/1407197): Investigate clearing the user data from
  // RenderProcessHostImpl::Cleanup.
  Destroy();
  // This instance has now been deleted.
}

// static
void NeutronApiSWIPCHandlerImpl::BindReceiver(
    int render_process_id,
    int64_t version_id,
    mojo::PendingAssociatedReceiver<mojom::NeutronApiIPC> receiver) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  auto* render_process_host =
      content::RenderProcessHost::FromID(render_process_id);
  if (!render_process_host) {
    return;
  }
  auto* service_worker_ipc_list = ServiceWorkerIPCList::Get(
      render_process_host, /*create_if_not_exists=*/true);
  service_worker_ipc_list->list.push_back(
      std::make_unique<NeutronApiSWIPCHandlerImpl>(
          render_process_host, version_id, std::move(receiver)));
}

}  // namespace neutron
