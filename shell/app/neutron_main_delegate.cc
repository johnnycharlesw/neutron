// Copyright (c) 2013 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "shell/app/neutron_main_delegate.h"

#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <utility>

#include "base/apple/bundle_locations.h"
#include "base/base_switches.h"
#include "base/command_line.h"
#include "base/debug/leak_annotations.h"
#include "base/debug/stack_trace.h"
#include "base/environment.h"
#include "base/files/file_util.h"
#include "base/logging.h"
#include "base/metrics/field_trial.h"
#include "base/path_service.h"
#include "base/strings/cstring_view.h"
#include "base/strings/string_number_conversions.cc"
#include "base/strings/string_util_internal.h"
#include "chrome/common/chrome_paths.h"
#include "chrome/common/chrome_switches.h"
#include "components/content_settings/core/common/content_settings_pattern.h"
#include "content/public/app/initialize_mojo_core.h"
#include "content/public/common/content_switches.h"
#include "crypto/hash.h"
#include "neutron/buildflags/buildflags.h"
#include "neutron/fuses.h"
#include "neutron/mas.h"
#include "neutron/snapshot_checksum.h"
#include "extensions/common/constants.h"
#include "gin/v8_initializer.h"
#include "sandbox/policy/switches.h"
#include "services/tracing/public/cpp/stack_sampling/tracing_sampler_profiler.h"
#include "shell/app/command_line_args.h"
#include "shell/app/neutron_content_client.h"
#include "shell/browser/neutron_browser_client.h"
#include "shell/browser/neutron_gpu_client.h"
#include "shell/browser/feature_list.h"
#include "shell/browser/relauncher.h"
#include "shell/common/application_info.h"
#include "shell/common/neutron_paths.h"
#include "shell/common/logging.h"
#include "shell/common/options_switches.h"
#include "shell/common/platform_util.h"
#include "shell/common/process_util.h"
#include "shell/common/thread_restrictions.h"
#include "shell/renderer/neutron_renderer_client.h"
#include "shell/renderer/neutron_sandboxed_renderer_client.h"
#include "shell/utility/neutron_content_utility_client.h"
#include "third_party/abseil-cpp/absl/types/variant.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/base/ui_base_switches.h"
#include "v8/include/v8-snapshot.h"

#if BUILDFLAG(IS_MAC)
#include "shell/app/neutron_main_delegate_mac.h"
#endif

#if BUILDFLAG(IS_WIN)
#include "base/win/win_util.h"
#include "chrome/child/v8_crashpad_support_win.h"
#endif

#if BUILDFLAG(IS_LINUX)
#include "base/nix/xdg_util.h"
#include "v8/include/v8-wasm-trap-handler-posix.h"
#include "v8/include/v8.h"
#endif

#if !IS_MAS_BUILD()
#include "components/crash/core/app/crash_switches.h"  // nogncheck
#include "components/crash/core/app/crashpad.h"        // nogncheck
#include "components/crash/core/common/crash_key.h"
#include "components/crash/core/common/crash_keys.h"
#include "shell/app/neutron_crash_reporter_client.h"
#include "shell/browser/api/neutron_api_crash_reporter.h"
#include "shell/common/crash_keys.h"
#endif

namespace neutron {

namespace {

constexpr std::string_view kRelauncherProcess = "relauncher";

constexpr base::cstring_view kNeutronDisableSandbox{
    "ELECTRON_DISABLE_SANDBOX"};
constexpr base::cstring_view kNeutronEnableStackDumping{
    "ELECTRON_ENABLE_STACK_DUMPING"};

// Returns true if this subprocess type needs the ResourceBundle initialized
// and resources loaded.
bool SubprocessNeedsResourceBundle(const std::string& process_type) {
  return
#if BUILDFLAG(IS_LINUX)
      // The zygote process opens the resources for the renderers.
      process_type == ::switches::kZygoteProcess ||
#endif
#if BUILDFLAG(IS_MAC)
      // Mac needs them too for scrollbar related images and for sandbox
      // profiles.
      process_type == ::switches::kGpuProcess ||
#endif
      process_type == ::switches::kRendererProcess ||
      process_type == ::switches::kUtilityProcess;
}

#if BUILDFLAG(IS_WIN)
void InvalidParameterHandler(const wchar_t*,
                             const wchar_t*,
                             const wchar_t*,
                             unsigned int,
                             uintptr_t) {
  // noop.
}
#endif

// TODO(nornagon): move path provider overriding to its own file in
// shell/common
bool NeutronPathProvider(int key, base::FilePath* result) {
  bool create_dir = false;
  base::FilePath cur;
  switch (key) {
    case chrome::DIR_USER_DATA:
      if (!base::PathService::Get(DIR_APP_DATA, &cur))
        return false;
      cur = cur.Append(base::FilePath::FromUTF8Unsafe(
          GetPossiblyOverriddenApplicationName()));
      create_dir = true;
      break;
    case DIR_CRASH_DUMPS:
      if (!base::PathService::Get(chrome::DIR_USER_DATA, &cur))
        return false;
      cur = cur.Append(FILE_PATH_LITERAL("Crashpad"));
      create_dir = true;
      break;
    case chrome::DIR_APP_DICTIONARIES:
      // TODO(nornagon): can we just default to using Chrome's logic here?
      if (!base::PathService::Get(DIR_SESSION_DATA, &cur))
        return false;
      cur = cur.Append(base::FilePath::FromUTF8Unsafe("Dictionaries"));
      create_dir = true;
      break;
    case DIR_SESSION_DATA:
      // By default and for backward, equivalent to DIR_USER_DATA.
      return base::PathService::Get(chrome::DIR_USER_DATA, result);
    case DIR_USER_CACHE: {
#if BUILDFLAG(IS_POSIX)
      int parent_key = base::DIR_CACHE;
#else
      // On Windows, there's no OS-level centralized location for caches, so
      // store the cache in the app data directory.
      int parent_key = base::DIR_ROAMING_APP_DATA;
#endif
      if (!base::PathService::Get(parent_key, &cur))
        return false;
      cur = cur.Append(base::FilePath::FromUTF8Unsafe(
          GetPossiblyOverriddenApplicationName()));
      create_dir = true;
      break;
    }
#if BUILDFLAG(IS_LINUX)
    case DIR_APP_DATA: {
      auto env = base::Environment::Create();
      cur = base::nix::GetXDGDirectory(
          env.get(), base::nix::kXdgConfigHomeEnvVar, base::nix::kDotConfigDir);
      break;
    }
#endif
#if BUILDFLAG(IS_WIN)
    case DIR_RECENT:
      if (!platform_util::GetFolderPath(DIR_RECENT, &cur))
        return false;
      create_dir = true;
      break;
#endif
    case DIR_APP_LOGS:
#if BUILDFLAG(IS_MAC)
      if (!base::PathService::Get(base::DIR_HOME, &cur))
        return false;
      cur = cur.Append(FILE_PATH_LITERAL("Library"));
      cur = cur.Append(FILE_PATH_LITERAL("Logs"));
      cur = cur.Append(base::FilePath::FromUTF8Unsafe(
          GetPossiblyOverriddenApplicationName()));
#else
      if (!base::PathService::Get(chrome::DIR_USER_DATA, &cur))
        return false;
      cur = cur.Append(base::FilePath::FromUTF8Unsafe("logs"));
#endif
      create_dir = true;
      break;
    default:
      return false;
  }

  // TODO(bauerb): http://crbug.com/259796
  ScopedAllowBlockingForNeutron allow_blocking;
  if (create_dir && !base::PathExists(cur) && !base::CreateDirectory(cur)) {
    return false;
  }

  *result = cur;

  return true;
}

void RegisterPathProvider() {
  base::PathService::RegisterProvider(NeutronPathProvider, PATH_START,
                                      PATH_END);
}

void ValidateV8Snapshot(v8::StartupData* data) {
  if (data->data &&
      neutron::fuses::IsEmbeddedAsarIntegrityValidationEnabled()) {
    CHECK_GT(data->raw_size, 0);
    UNSAFE_BUFFERS({
      base::span<const char> span_data(
          data->data, static_cast<unsigned long>(data->raw_size));
      CHECK(base::ToLowerASCII(base::HexEncode(
                crypto::hash::Sha256(base::as_bytes(span_data)))) ==
            neutron::snapshot_checksum::kChecksum);
    })
  }
}

}  // namespace

std::string LoadResourceBundle(const std::string& locale) {
  const bool initialized = ui::ResourceBundle::HasSharedInstance();
  DCHECK(!initialized);

  // Load other resource files.
  base::FilePath pak_dir;
#if BUILDFLAG(IS_MAC)
  pak_dir =
      base::apple::FrameworkBundlePath().Append(FILE_PATH_LITERAL("Resources"));
#else
  base::PathService::Get(base::DIR_ASSETS, &pak_dir);
#endif

  std::string loaded_locale = ui::ResourceBundle::InitSharedInstanceWithLocale(
      locale, nullptr, ui::ResourceBundle::LOAD_COMMON_RESOURCES);
  ui::ResourceBundle& bundle = ui::ResourceBundle::GetSharedInstance();
  bundle.AddDataPackFromPath(pak_dir.Append(FILE_PATH_LITERAL("resources.pak")),
                             ui::kScaleFactorNone);
  return loaded_locale;
}

NeutronMainDelegate::NeutronMainDelegate() {
  gin::SetV8SnapshotValidator(base::BindRepeating(&ValidateV8Snapshot));
}

NeutronMainDelegate::~NeutronMainDelegate() = default;

const char* const NeutronMainDelegate::kNonWildcardDomainNonPortSchemes[] = {
    extensions::kExtensionScheme};
const size_t NeutronMainDelegate::kNonWildcardDomainNonPortSchemesSize =
    std::size(kNonWildcardDomainNonPortSchemes);

std::optional<int> NeutronMainDelegate::BasicStartupComplete() {
  auto* command_line = base::CommandLine::ForCurrentProcess();

#if BUILDFLAG(IS_WIN)
  v8_crashpad_support::SetUp();

  // On Windows the terminal returns immediately, so we add a new line to
  // prevent output in the same line as the prompt.
  if (IsBrowserProcess())
    std::wcout << std::endl;
#endif  // !BUILDFLAG(IS_WIN)

  auto env = base::Environment::Create();

  // Enable convenient stack printing. This is enabled by default in
  // non-official builds.
  if (env->HasVar(kNeutronEnableStackDumping))
    base::debug::EnableInProcessStackDumping();

  if (env->HasVar(kNeutronDisableSandbox))
    command_line->AppendSwitch(sandbox::policy::switches::kNoSandbox);

  tracing_sampler_profiler_ =
      tracing::TracingSamplerProfiler::CreateOnMainThread();

  chrome::RegisterPathProvider();
  neutron::RegisterPathProvider();

#if BUILDFLAG(ENABLE_ELECTRON_EXTENSIONS)
  ContentSettingsPattern::SetNonWildcardDomainNonPortSchemes(
      kNonWildcardDomainNonPortSchemes, kNonWildcardDomainNonPortSchemesSize);
#endif

#if BUILDFLAG(IS_WIN)
  // Ignore invalid parameter errors.
  _set_invalid_parameter_handler(InvalidParameterHandler);
  // Disable the ActiveVerifier, which is used by Chrome to track possible
  // bugs, but no use in Neutron.
  base::win::DisableHandleVerifier();

  if (IsBrowserProcess())
    base::win::PinUser32();
#endif

#if BUILDFLAG(IS_LINUX)
  // Check for --no-sandbox parameter when running as root.
  if (getuid() == 0 && IsSandboxEnabled(command_line))
    LOG(FATAL) << "Running as root without --"
               << sandbox::policy::switches::kNoSandbox
               << " is not supported. See https://crbug.com/638180.";
#endif

#if IS_MAS_BUILD()
  // In MAS build we are using --disable-remote-core-animation.
  //
  // According to ccameron:
  // If you're running with --disable-remote-core-animation, you may want to
  // also run with --disable-gpu-memory-buffer-compositor-resources as well.
  // That flag makes it so we use regular GL textures instead of IOSurfaces
  // for compositor resources. IOSurfaces are very heavyweight to
  // create/destroy, but they can be displayed directly by CoreAnimation (and
  // --disable-remote-core-animation makes it so we don't use this property,
  // so they're just heavyweight with no upside).
  command_line->AppendSwitch(
      ::switches::kDisableGpuMemoryBufferCompositorResources);
#endif

  return std::nullopt;
}

void NeutronMainDelegate::PreSandboxStartup() {
  auto* command_line = base::CommandLine::ForCurrentProcess();
  std::string process_type = GetProcessType();

  base::FilePath user_data_dir =
      command_line->GetSwitchValuePath(::switches::kUserDataDir);
  if (!user_data_dir.empty()) {
    base::PathService::OverrideAndCreateIfNeeded(chrome::DIR_USER_DATA,
                                                 user_data_dir, false, true);
  }

#if !BUILDFLAG(IS_WIN)
  // For windows we call InitLogging later, after the sandbox is initialized.
  //
  // On Linux, we force a "preinit" in the zygote (i.e. never log to a default
  // log file), because the zygote is booted prior to JS running, so it can't
  // know the correct user-data directory. (And, further, accessing the
  // application name on Linux can cause glib calls that end up spawning
  // threads, which if done before the zygote is booted, causes a CHECK().)
  logging::InitNeutronLogging(
      *command_line,
      /* is_preinit = */ IsBrowserProcess() || IsZygoteProcess());
#endif

#if !IS_MAS_BUILD()
  crash_reporter::InitializeCrashKeys();
#endif

  // Initialize ResourceBundle which handles files loaded from external
  // sources. The language should have been passed in to us from the
  // browser process as a command line flag.
  if (SubprocessNeedsResourceBundle(process_type)) {
    std::string locale = command_line->GetSwitchValueASCII(::switches::kLang);
    LoadResourceBundle(locale);
  }

#if BUILDFLAG(IS_WIN) || (BUILDFLAG(IS_MAC) && !IS_MAS_BUILD())
  // In the main process, we wait for JS to call crashReporter.start() before
  // initializing crashpad. If we're in the renderer, we want to initialize it
  // immediately at boot.
  if (!IsBrowserProcess()) {
    NeutronCrashReporterClient::Create();
    crash_reporter::InitializeCrashpad(false, process_type);
  }
#endif

#if BUILDFLAG(IS_LINUX)
  // Zygote needs to call InitCrashReporter() in RunZygote().
  if (!IsZygoteProcess() && !IsBrowserProcess()) {
    NeutronCrashReporterClient::Create();
    if (command_line->HasSwitch(
            crash_reporter::switches::kCrashpadHandlerPid)) {
      crash_reporter::InitializeCrashpad(false, process_type);
      crash_reporter::SetFirstChanceExceptionHandler(
          v8::TryHandleWebAssemblyTrapPosix);
    }
  }
#endif

#if !IS_MAS_BUILD()
  crash_keys::SetCrashKeysFromCommandLine(*command_line);
  crash_keys::SetPlatformCrashKey();
#endif

  if (IsBrowserProcess()) {
    // Only append arguments for browser process.

    // Allow file:// URIs to read other file:// URIs by default.
    command_line->AppendSwitch(::switches::kAllowFileAccessFromFiles);

#if BUILDFLAG(IS_MAC)
    // Enable AVFoundation.
    command_line->AppendSwitch("enable-avfoundation");
#endif
  }
}

void NeutronMainDelegate::SandboxInitialized(const std::string& process_type) {
#if BUILDFLAG(IS_WIN)
  logging::InitNeutronLogging(*base::CommandLine::ForCurrentProcess(),
                               /* is_preinit = */ process_type.empty());
#endif
}

std::optional<int> NeutronMainDelegate::PreBrowserMain() {
  // This is initialized early because the service manager reads some feature
  // flags and we need to make sure the feature list is initialized before the
  // service manager reads the features.
  if (!base::FieldTrialList::GetInstance()) {
    base::FieldTrialList* leaked_field_trial_list = new base::FieldTrialList();
    ANNOTATE_LEAKING_OBJECT_PTR(leaked_field_trial_list);
    std::ignore = leaked_field_trial_list;
  }
  InitializeFeatureList();
  // Initialize mojo core as soon as we have a valid feature list
  content::InitializeMojoCore();
#if BUILDFLAG(IS_MAC)
  RegisterAtomCrApp();
#endif
#if BUILDFLAG(IS_LINUX)
  // Set the global activation token sent as an environment variable.
  auto env = base::Environment::Create();
  base::nix::ExtractXdgActivationTokenFromEnv(*env);
#endif
  return std::nullopt;
}

std::string_view NeutronMainDelegate::GetBrowserV8SnapshotFilename() {
  bool load_browser_process_specific_v8_snapshot =
      IsBrowserProcess() &&
      neutron::fuses::IsLoadBrowserProcessSpecificV8SnapshotEnabled();
  if (load_browser_process_specific_v8_snapshot) {
    return "browser_v8_context_snapshot.bin";
  }
  return ContentMainDelegate::GetBrowserV8SnapshotFilename();
}

content::ContentClient* NeutronMainDelegate::CreateContentClient() {
  content_client_ = std::make_unique<NeutronContentClient>();
  return content_client_.get();
}

content::ContentBrowserClient*
NeutronMainDelegate::CreateContentBrowserClient() {
  browser_client_ = std::make_unique<NeutronBrowserClient>();
  return browser_client_.get();
}

content::ContentGpuClient* NeutronMainDelegate::CreateContentGpuClient() {
  gpu_client_ = std::make_unique<NeutronGpuClient>();
  return gpu_client_.get();
}

content::ContentRendererClient*
NeutronMainDelegate::CreateContentRendererClient() {
  auto* command_line = base::CommandLine::ForCurrentProcess();

  if (IsSandboxEnabled(command_line)) {
    renderer_client_ = std::make_unique<NeutronSandboxedRendererClient>();
  } else {
    renderer_client_ = std::make_unique<NeutronRendererClient>();
  }

  return renderer_client_.get();
}

content::ContentUtilityClient*
NeutronMainDelegate::CreateContentUtilityClient() {
  utility_client_ = std::make_unique<NeutronContentUtilityClient>();
  return utility_client_.get();
}

std::variant<int, content::MainFunctionParams> NeutronMainDelegate::RunProcess(
    const std::string& process_type,
    content::MainFunctionParams main_function_params) {
  if (process_type == kRelauncherProcess)
    return relauncher::RelauncherMain(main_function_params);
  else
    return std::move(main_function_params);
}

bool NeutronMainDelegate::ShouldCreateFeatureList(InvokedIn invoked_in) {
  return std::holds_alternative<InvokedInChildProcess>(invoked_in);
}

bool NeutronMainDelegate::ShouldInitializeMojo(InvokedIn invoked_in) {
  return ShouldCreateFeatureList(invoked_in);
}

bool NeutronMainDelegate::ShouldLockSchemeRegistry() {
  return false;
}

#if BUILDFLAG(IS_LINUX)
void NeutronMainDelegate::ZygoteForked() {
  // Needs to be called after we have DIR_USER_DATA.  BrowserMain sets
  // this up for the browser process in a different manner.
  NeutronCrashReporterClient::Create();
  const base::CommandLine* command_line =
      base::CommandLine::ForCurrentProcess();
  std::string process_type =
      command_line->GetSwitchValueASCII(::switches::kProcessType);
  if (command_line->HasSwitch(crash_reporter::switches::kCrashpadHandlerPid)) {
    crash_reporter::InitializeCrashpad(false, process_type);
    crash_reporter::SetFirstChanceExceptionHandler(
        v8::TryHandleWebAssemblyTrapPosix);
  }

  // Reset the command line for the newly spawned process.
  crash_keys::SetCrashKeysFromCommandLine(*command_line);
}
#endif  // BUILDFLAG(IS_LINUX)

}  // namespace neutron
