// Copyright (c) 2022 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "shell/browser/webauthn/neutron_authenticator_request_delegate.h"

namespace neutron {

NeutronWebAuthenticationDelegate::~NeutronWebAuthenticationDelegate() =
    default;

bool NeutronWebAuthenticationDelegate::SupportsResidentKeys(
    content::RenderFrameHost* render_frame_host) {
  return true;
}

}  // namespace neutron
