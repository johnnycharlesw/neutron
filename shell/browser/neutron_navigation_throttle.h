// Copyright (c) 2018 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ELECTRON_SHELL_BROWSER_ELECTRON_NAVIGATION_THROTTLE_H_
#define ELECTRON_SHELL_BROWSER_ELECTRON_NAVIGATION_THROTTLE_H_

#include "content/public/browser/navigation_throttle.h"

namespace neutron {

class NeutronNavigationThrottle : public content::NavigationThrottle {
 public:
  explicit NeutronNavigationThrottle(
      content::NavigationThrottleRegistry& registry);
  ~NeutronNavigationThrottle() override;

  // disable copy
  NeutronNavigationThrottle(const NeutronNavigationThrottle&) = delete;
  NeutronNavigationThrottle& operator=(const NeutronNavigationThrottle&) =
      delete;

  // content::NavigationThrottle
  NeutronNavigationThrottle::ThrottleCheckResult WillStartRequest() override;
  NeutronNavigationThrottle::ThrottleCheckResult WillRedirectRequest()
      override;

  const char* GetNameForLogging() override;
};

}  // namespace neutron

#endif  // ELECTRON_SHELL_BROWSER_ELECTRON_NAVIGATION_THROTTLE_H_
