// Copyright (c) 2022 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ELECTRON_SHELL_COMMON_PLUGIN_INFO_H_
#define ELECTRON_SHELL_COMMON_PLUGIN_INFO_H_

#include <vector>

#include "content/public/common/webplugininfo.h"
#include "neutron/buildflags/buildflags.h"

namespace neutron {

void GetInternalPlugins(std::vector<content::WebPluginInfo>* plugins);

#if BUILDFLAG(ENABLE_PDF_VIEWER)
content::WebPluginInfo GetPDFPluginInfo();
#endif  // BUILDFLAG(ENABLE_PDF_VIEWER)

}  // namespace neutron

#endif  // ELECTRON_SHELL_COMMON_PLUGIN_INFO_H_
