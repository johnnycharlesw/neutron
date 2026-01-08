// Copyright (c) 2025 Microsoft, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "shell/browser/neutron_preconnect_manager_delegate.h"

namespace neutron {

NeutronPreconnectManagerDelegate::NeutronPreconnectManagerDelegate() =
    default;
NeutronPreconnectManagerDelegate::~NeutronPreconnectManagerDelegate() =
    default;

bool NeutronPreconnectManagerDelegate::IsPreconnectEnabled() {
  return true;
}

}  // namespace neutron
