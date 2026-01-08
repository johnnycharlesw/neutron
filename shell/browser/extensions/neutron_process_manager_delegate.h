// Copyright 2019 Slack Technologies, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ELECTRON_SHELL_BROWSER_EXTENSIONS_ELECTRON_PROCESS_MANAGER_DELEGATE_H_
#define ELECTRON_SHELL_BROWSER_EXTENSIONS_ELECTRON_PROCESS_MANAGER_DELEGATE_H_

#include "extensions/browser/process_manager_delegate.h"

class Browser;
class Profile;

namespace extensions {

// Support for ProcessManager. Controls cases where Neutron wishes to disallow
// extension background pages or defer their creation.
class NeutronProcessManagerDelegate : public ProcessManagerDelegate {
 public:
  NeutronProcessManagerDelegate();
  ~NeutronProcessManagerDelegate() override;

  // disable copy
  NeutronProcessManagerDelegate(const NeutronProcessManagerDelegate&) =
      delete;
  NeutronProcessManagerDelegate& operator=(
      const NeutronProcessManagerDelegate&) = delete;

  // ProcessManagerDelegate implementation:
  bool AreBackgroundPagesAllowedForContext(
      content::BrowserContext* context) const override;
  bool IsExtensionBackgroundPageAllowed(
      content::BrowserContext* context,
      const Extension& extension) const override;
  bool DeferCreatingStartupBackgroundHosts(
      content::BrowserContext* context) const override;
};

}  // namespace extensions

#endif  // ELECTRON_SHELL_BROWSER_EXTENSIONS_ELECTRON_PROCESS_MANAGER_DELEGATE_H_
