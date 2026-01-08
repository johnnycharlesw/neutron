// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "shell/browser/extensions/neutron_kiosk_delegate.h"

namespace neutron {

NeutronKioskDelegate::NeutronKioskDelegate() = default;

NeutronKioskDelegate::~NeutronKioskDelegate() = default;

bool NeutronKioskDelegate::IsAutoLaunchedKioskApp(
    const extensions::ExtensionId& id) const {
  return false;
}

}  // namespace neutron
