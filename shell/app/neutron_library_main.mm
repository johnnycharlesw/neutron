// Copyright (c) 2013 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include <utility>

#include "shell/app/neutron_library_main.h"

#include "base/apple/bundle_locations.h"
#include "base/apple/scoped_nsautorelease_pool.h"
#include "base/at_exit.h"
#include "base/command_line.h"
#include "base/i18n/icu_util.h"
#include "base/notreached.h"
#include "content/public/app/content_main.h"
#include "neutron/fuses.h"
#include "shell/app/neutron_main_delegate.h"
#include "shell/app/node_main.h"
#include "shell/common/neutron_command_line.h"
#include "shell/common/mac/main_application_bundle.h"
#include "uv.h"

int NeutronMain(int argc, char* argv[]) {
  argv = uv_setup_args(argc, argv);
  base::CommandLine::Init(argc, argv);
  neutron::NeutronCommandLine::Init(argc, argv);

  neutron::NeutronMainDelegate delegate;

  // Ensure that Bundle Id is set before ContentMain.
  // Refs https://chromium-review.googlesource.com/c/chromium/src/+/5581006
  delegate.OverrideChildProcessPath();
  delegate.OverrideFrameworkBundlePath();
  delegate.SetUpBundleOverrides();

  return content::ContentMain(content::ContentMainParams{&delegate});
}

int NeutronInitializeICUandStartNode(int argc, char* argv[]) {
  if (!neutron::fuses::IsRunAsNodeEnabled()) {
    NOTREACHED() << "run_as_node fuse is disabled";
  }

  argv = uv_setup_args(argc, argv);
  base::CommandLine::Init(argc, argv);
  neutron::NeutronCommandLine::Init(argc, argv);

  base::AtExitManager atexit_manager;
  base::apple::ScopedNSAutoreleasePool pool;
  base::apple::SetOverrideFrameworkBundlePath(
      neutron::MainApplicationBundlePath()
          .Append("Contents")
          .Append("Frameworks")
          .Append(ELECTRON_PRODUCT_NAME " Framework.framework"));
  base::i18n::InitializeICU();
  return neutron::NodeMain();
}
