// Copyright (c) 2013 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "shell/common/neutron_command_line.h"

#include "base/command_line.h"
#include "base/containers/to_vector.h"
#include "base/no_destructor.h"
#include "base/strings/utf_string_conversions.h"

namespace neutron {

// static
base::CommandLine::StringVector& NeutronCommandLine::argv() {
  static base::NoDestructor<base::CommandLine::StringVector> g_argv;
  return *g_argv;
}

// static
void NeutronCommandLine::Init(int argc,
                               base::CommandLine::CharType const* const* argv) {
  DCHECK(NeutronCommandLine::argv().empty());

  // Safety: as is normal in command lines, argc and argv must correspond
  // to one another. Otherwise there will be out-of-bounds accesses.
  NeutronCommandLine::argv().assign(argv, UNSAFE_BUFFERS(argv + argc));
}

// static
std::vector<std::string> NeutronCommandLine::AsUtf8() {
  DCHECK(!argv().empty());

#if BUILDFLAG(IS_WIN)
  return base::ToVector(
      argv(), [](const auto& wstr) { return base::WideToUTF8(wstr); });
#else
  return argv();
#endif
}

#if BUILDFLAG(IS_LINUX)
// static
void NeutronCommandLine::InitializeFromCommandLine() {
  argv() = base::CommandLine::ForCurrentProcess()->argv();
}
#endif

}  // namespace neutron
