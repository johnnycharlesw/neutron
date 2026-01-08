// Copyright (c) 2024 Salesforce, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "shell/browser/api/neutron_api_desktop_capturer.h"

namespace neutron::api {

// static
bool DesktopCapturer::IsDisplayMediaSystemPickerAvailable() {
  if (@available(macOS 15.0, *)) {
    return true;
  }
  return false;
}

}  // namespace neutron::api
