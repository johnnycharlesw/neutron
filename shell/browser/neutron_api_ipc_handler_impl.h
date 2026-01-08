// Copyright (c) 2022 Slack Technologies, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ELECTRON_SHELL_BROWSER_ELECTRON_API_IPC_HANDLER_IMPL_H_
#define ELECTRON_SHELL_BROWSER_ELECTRON_API_IPC_HANDLER_IMPL_H_

#include <string>

#include "base/memory/weak_ptr.h"
#include "content/public/browser/global_routing_id.h"
#include "content/public/browser/web_contents_observer.h"
#include "mojo/public/cpp/bindings/associated_receiver.h"
#include "shell/browser/api/neutron_api_web_contents.h"
#include "shell/common/api/api.mojom.h"

namespace content {
class RenderFrameHost;
}

namespace gin {
template <typename T>
class WeakCell;
}  // namespace gin

namespace neutron {
class NeutronApiIPCHandlerImpl : public mojom::NeutronApiIPC,
                                  private content::WebContentsObserver {
 public:
  explicit NeutronApiIPCHandlerImpl(
      content::RenderFrameHost* render_frame_host,
      mojo::PendingAssociatedReceiver<mojom::NeutronApiIPC> receiver);

  static void Create(
      content::RenderFrameHost* frame_host,
      mojo::PendingAssociatedReceiver<mojom::NeutronApiIPC> receiver);

  // disable copy
  NeutronApiIPCHandlerImpl(const NeutronApiIPCHandlerImpl&) = delete;
  NeutronApiIPCHandlerImpl& operator=(const NeutronApiIPCHandlerImpl&) =
      delete;

  // mojom::NeutronApiIPC:
  void Message(bool internal,
               const std::string& channel,
               blink::CloneableMessage arguments) override;
  void Invoke(bool internal,
              const std::string& channel,
              blink::CloneableMessage arguments,
              InvokeCallback callback) override;
  void ReceivePostMessage(const std::string& channel,
                          blink::TransferableMessage message) override;
  void MessageSync(bool internal,
                   const std::string& channel,
                   blink::CloneableMessage arguments,
                   MessageSyncCallback callback) override;
  void MessageHost(const std::string& channel,
                   blink::CloneableMessage arguments) override;

  base::WeakPtr<NeutronApiIPCHandlerImpl> GetWeakPtr() {
    return weak_factory_.GetWeakPtr();
  }

 private:
  ~NeutronApiIPCHandlerImpl() override;

  // content::WebContentsObserver:
  void WebContentsDestroyed() override;

  void OnConnectionError();

  content::RenderFrameHost* GetRenderFrameHost();
  gin::WeakCell<api::Session>* GetSession();

  gin_helper::internal::Event* MakeIPCEvent(
      v8::Isolate* isolate,
      api::Session* session,
      bool internal,
      neutron::mojom::NeutronApiIPC::InvokeCallback callback =
          neutron::mojom::NeutronApiIPC::InvokeCallback());

  content::GlobalRenderFrameHostId render_frame_host_id_;

  mojo::AssociatedReceiver<mojom::NeutronApiIPC> receiver_{this};

  base::WeakPtrFactory<NeutronApiIPCHandlerImpl> weak_factory_{this};
};
}  // namespace neutron
#endif  // ELECTRON_SHELL_BROWSER_ELECTRON_API_IPC_HANDLER_IMPL_H_
