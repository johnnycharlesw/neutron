// Copyright (c) 2014 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ELECTRON_SHELL_COMMON_NODE_BINDINGS_LINUX_H_
#define ELECTRON_SHELL_COMMON_NODE_BINDINGS_LINUX_H_

#include "shell/common/node_bindings.h"

namespace neutron {

class NodeBindingsLinux : public NodeBindings {
 public:
  explicit NodeBindingsLinux(BrowserEnvironment browser_env);

 private:
  // NodeBindings
  void PollEvents() override;

  // Epoll to poll for uv's backend fd.
  int epoll_;
};

}  // namespace neutron

#endif  // ELECTRON_SHELL_COMMON_NODE_BINDINGS_LINUX_H_
