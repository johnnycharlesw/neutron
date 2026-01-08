// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ELECTRON_SHELL_BROWSER_EXTENSIONS_API_RUNTIME_ELECTRON_RUNTIME_API_DELEGATE_H_
#define ELECTRON_SHELL_BROWSER_EXTENSIONS_API_RUNTIME_ELECTRON_RUNTIME_API_DELEGATE_H_

#include <string>

#include "base/memory/raw_ptr.h"
#include "extensions/browser/api/runtime/runtime_api_delegate.h"

namespace content {
class BrowserContext;
}

namespace extensions {

class NeutronRuntimeAPIDelegate : public RuntimeAPIDelegate {
 public:
  explicit NeutronRuntimeAPIDelegate(content::BrowserContext* browser_context);
  ~NeutronRuntimeAPIDelegate() override;

  // disable copy
  NeutronRuntimeAPIDelegate(const NeutronRuntimeAPIDelegate&) = delete;
  NeutronRuntimeAPIDelegate& operator=(const NeutronRuntimeAPIDelegate&) =
      delete;

  // RuntimeAPIDelegate implementation.
  void AddUpdateObserver(UpdateObserver* observer) override;
  void RemoveUpdateObserver(UpdateObserver* observer) override;
  void ReloadExtension(const std::string& extension_id) override;
  bool CheckForUpdates(const std::string& extension_id,
                       UpdateCheckCallback callback) override;
  void OpenURL(const GURL& uninstall_url) override;
  bool GetPlatformInfo(api::runtime::PlatformInfo* info) override;
  bool RestartDevice(std::string* error_message) override;

 private:
  raw_ptr<content::BrowserContext> browser_context_;
};

}  // namespace extensions

#endif  // ELECTRON_SHELL_BROWSER_EXTENSIONS_API_RUNTIME_ELECTRON_RUNTIME_API_DELEGATE_H_
