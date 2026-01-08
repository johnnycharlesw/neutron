// Copyright (c) 2019 Slack Technologies, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "shell/browser/extensions/neutron_extensions_api_client.h"

#include <memory>
#include <string>

#include "neutron/buildflags/buildflags.h"
#include "extensions/browser/guest_view/extensions_guest_view_manager_delegate.h"
#include "extensions/browser/guest_view/mime_handler_view/mime_handler_view_guest_delegate.h"
#include "printing/buildflags/buildflags.h"
#include "shell/browser/api/neutron_api_web_contents.h"
#include "shell/browser/extensions/api/management/neutron_management_api_delegate.h"
#include "shell/browser/extensions/neutron_extension_web_contents_observer.h"
#include "shell/browser/extensions/neutron_messaging_delegate.h"
#include "v8/include/v8.h"

#if BUILDFLAG(ENABLE_PRINTING)
#include "components/printing/browser/print_manager_utils.h"
#include "shell/browser/printing/print_view_manager_neutron.h"
#endif

namespace extensions {

class NeutronGuestViewManagerDelegate
    : public ExtensionsGuestViewManagerDelegate {
 public:
  NeutronGuestViewManagerDelegate() : ExtensionsGuestViewManagerDelegate() {}
  ~NeutronGuestViewManagerDelegate() override = default;

  // disable copy
  NeutronGuestViewManagerDelegate(const NeutronGuestViewManagerDelegate&) =
      delete;
  NeutronGuestViewManagerDelegate& operator=(
      const NeutronGuestViewManagerDelegate&) = delete;

  // GuestViewManagerDelegate:
  void OnGuestAdded(content::WebContents* guest_web_contents) const override {
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::HandleScope scope(isolate);
    neutron::api::WebContents::FromOrCreate(isolate, guest_web_contents);
  }
};

class NeutronMimeHandlerViewGuestDelegate
    : public MimeHandlerViewGuestDelegate {
 public:
  NeutronMimeHandlerViewGuestDelegate() = default;
  ~NeutronMimeHandlerViewGuestDelegate() override = default;

  // disable copy
  NeutronMimeHandlerViewGuestDelegate(
      const NeutronMimeHandlerViewGuestDelegate&) = delete;
  NeutronMimeHandlerViewGuestDelegate& operator=(
      const NeutronMimeHandlerViewGuestDelegate&) = delete;

  // MimeHandlerViewGuestDelegate.
  bool HandleContextMenu(content::RenderFrameHost& render_frame_host,
                         const content::ContextMenuParams& params) override {
    auto* web_contents =
        content::WebContents::FromRenderFrameHost(&render_frame_host);
    if (!web_contents)
      return true;

    neutron::api::WebContents* api_web_contents =
        neutron::api::WebContents::From(
            web_contents->GetOutermostWebContents());
    if (api_web_contents)
      api_web_contents->HandleContextMenu(render_frame_host, params);
    return true;
  }

  void RecordLoadMetric(bool in_main_frame,
                        const std::string& mime_type,
                        content::BrowserContext* browser_context) override {}
};

NeutronExtensionsAPIClient::NeutronExtensionsAPIClient() = default;
NeutronExtensionsAPIClient::~NeutronExtensionsAPIClient() = default;

MessagingDelegate* NeutronExtensionsAPIClient::GetMessagingDelegate() {
  if (!messaging_delegate_)
    messaging_delegate_ = std::make_unique<NeutronMessagingDelegate>();
  return messaging_delegate_.get();
}

void NeutronExtensionsAPIClient::AttachWebContentsHelpers(
    content::WebContents* web_contents) const {
#if BUILDFLAG(ENABLE_PRINTING)
  neutron::PrintViewManagerNeutron::CreateForWebContents(web_contents);
  printing::CreateCompositeClientIfNeeded(web_contents, std::string());
#endif

  extensions::NeutronExtensionWebContentsObserver::CreateForWebContents(
      web_contents);
}

ManagementAPIDelegate*
NeutronExtensionsAPIClient::CreateManagementAPIDelegate() const {
  return new NeutronManagementAPIDelegate;
}

std::unique_ptr<MimeHandlerViewGuestDelegate>
NeutronExtensionsAPIClient::CreateMimeHandlerViewGuestDelegate(
    MimeHandlerViewGuest* guest) const {
  return std::make_unique<NeutronMimeHandlerViewGuestDelegate>();
}

std::unique_ptr<guest_view::GuestViewManagerDelegate>
NeutronExtensionsAPIClient::CreateGuestViewManagerDelegate() const {
  return std::make_unique<NeutronGuestViewManagerDelegate>();
}

}  // namespace extensions
