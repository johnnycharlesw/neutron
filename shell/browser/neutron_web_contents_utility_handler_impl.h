// Copyright (c) 2022 Slack Technologies, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ELECTRON_SHELL_BROWSER_ELECTRON_WEB_CONTENTS_UTILITY_HANDLER_IMPL_H_
#define ELECTRON_SHELL_BROWSER_ELECTRON_WEB_CONTENTS_UTILITY_HANDLER_IMPL_H_

#include "base/memory/weak_ptr.h"
#include "content/public/browser/global_routing_id.h"
#include "content/public/browser/web_contents_observer.h"
#include "neutron/shell/common/web_contents_utility.mojom.h"
#include "mojo/public/cpp/bindings/associated_receiver.h"
#include "shell/browser/api/neutron_api_web_contents.h"

namespace content {
class RenderFrameHost;
}

namespace neutron {
class NeutronWebContentsUtilityHandlerImpl
    : public mojom::NeutronWebContentsUtility,
      private content::WebContentsObserver {
 public:
  explicit NeutronWebContentsUtilityHandlerImpl(
      content::RenderFrameHost* render_frame_host,
      mojo::PendingAssociatedReceiver<mojom::NeutronWebContentsUtility>
          receiver);

  static void Create(
      content::RenderFrameHost* frame_host,
      mojo::PendingAssociatedReceiver<mojom::NeutronWebContentsUtility>
          receiver);

  // disable copy
  NeutronWebContentsUtilityHandlerImpl(
      const NeutronWebContentsUtilityHandlerImpl&) = delete;
  NeutronWebContentsUtilityHandlerImpl& operator=(
      const NeutronWebContentsUtilityHandlerImpl&) = delete;

  // mojom::NeutronWebContentsUtility:
  void OnFirstNonEmptyLayout() override;
  void SetTemporaryZoomLevel(double level) override;
  void CanAccessClipboardDeprecated(
      mojom::PermissionName name,
      const blink::LocalFrameToken& frame_token,
      CanAccessClipboardDeprecatedCallback callback) override;

  base::WeakPtr<NeutronWebContentsUtilityHandlerImpl> GetWeakPtr() {
    return weak_factory_.GetWeakPtr();
  }

 private:
  ~NeutronWebContentsUtilityHandlerImpl() override;

  // content::WebContentsObserver:
  void RenderFrameDeleted(content::RenderFrameHost* render_frame_host) override;

  void OnConnectionError();

  content::RenderFrameHost* GetRenderFrameHost();

  content::GlobalRenderFrameHostToken render_frame_host_token_;

  mojo::AssociatedReceiver<mojom::NeutronWebContentsUtility> receiver_{this};

  base::WeakPtrFactory<NeutronWebContentsUtilityHandlerImpl> weak_factory_{
      this};
};
}  // namespace neutron
#endif  // ELECTRON_SHELL_BROWSER_ELECTRON_WEB_CONTENTS_UTILITY_HANDLER_IMPL_H_
