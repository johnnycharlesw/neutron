// Copyright (c) 2022 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ELECTRON_SHELL_BROWSER_WEBAUTHN_ELECTRON_AUTHENTICATOR_REQUEST_DELEGATE_H_
#define ELECTRON_SHELL_BROWSER_WEBAUTHN_ELECTRON_AUTHENTICATOR_REQUEST_DELEGATE_H_

#include "content/public/browser/web_authentication_delegate.h"

namespace neutron {

// Modified from chrome/browser/webauthn/chrome_authenticator_request_delegate.h
class NeutronWebAuthenticationDelegate
    : public content::WebAuthenticationDelegate {
 public:
  ~NeutronWebAuthenticationDelegate() override;

  // content::WebAuthenticationDelegate
  bool SupportsResidentKeys(
      content::RenderFrameHost* render_frame_host) override;
};

}  // namespace neutron
#endif  // ELECTRON_SHELL_BROWSER_WEBAUTHN_ELECTRON_AUTHENTICATOR_REQUEST_DELEGATE_H_
