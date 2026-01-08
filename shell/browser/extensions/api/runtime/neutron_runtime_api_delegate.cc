// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "shell/browser/extensions/api/runtime/neutron_runtime_api_delegate.h"

#include <string>
#include <string_view>

#include "components/update_client/update_query_params.h"
#include "extensions/common/api/runtime.h"
#include "shell/browser/extensions/neutron_extension_system.h"

using extensions::api::runtime::PlatformInfo;

namespace extensions {

NeutronRuntimeAPIDelegate::NeutronRuntimeAPIDelegate(
    content::BrowserContext* browser_context)
    : browser_context_(browser_context) {
  DCHECK(browser_context_);
}

NeutronRuntimeAPIDelegate::~NeutronRuntimeAPIDelegate() = default;

void NeutronRuntimeAPIDelegate::AddUpdateObserver(UpdateObserver* observer) {}

void NeutronRuntimeAPIDelegate::RemoveUpdateObserver(
    UpdateObserver* observer) {}

void NeutronRuntimeAPIDelegate::ReloadExtension(
    const std::string& extension_id) {
  static_cast<NeutronExtensionSystem*>(ExtensionSystem::Get(browser_context_))
      ->ReloadExtension(extension_id);
}

bool NeutronRuntimeAPIDelegate::CheckForUpdates(
    const std::string& extension_id,
    UpdateCheckCallback callback) {
  LOG(INFO) << "chrome.runtime.requestUpdateCheck is not supported in Neutron";
  return false;
}

void NeutronRuntimeAPIDelegate::OpenURL(const GURL& uninstall_url) {
  LOG(INFO) << "chrome.runtime.openURL is not supported in Neutron";
}

bool NeutronRuntimeAPIDelegate::GetPlatformInfo(PlatformInfo* info) {
  if (const std::string_view os = update_client::UpdateQueryParams::GetOS();
      os == "mac") {
    info->os = extensions::api::runtime::PlatformOs::kMac;
  } else if (os == "win") {
    info->os = extensions::api::runtime::PlatformOs::kWin;
  } else if (os == "linux") {
    info->os = extensions::api::runtime::PlatformOs::kLinux;
  } else if (os == "openbsd") {
    info->os = extensions::api::runtime::PlatformOs::kOpenbsd;
  } else {
    NOTREACHED();
  }

  if (const std::string_view arch = update_client::UpdateQueryParams::GetArch();
      arch == "arm") {
    info->arch = extensions::api::runtime::PlatformArch::kArm;
  } else if (arch == "arm64") {
    info->arch = extensions::api::runtime::PlatformArch::kArm64;
  } else if (arch == "x86") {
    info->arch = extensions::api::runtime::PlatformArch::kX86_32;
  } else if (arch == "x64") {
    info->arch = extensions::api::runtime::PlatformArch::kX86_64;
  } else {
    NOTREACHED();
  }

  return true;
}

bool NeutronRuntimeAPIDelegate::RestartDevice(std::string* error_message) {
  *error_message = "Restart is not supported in Neutron";
  return false;
}

}  // namespace extensions
