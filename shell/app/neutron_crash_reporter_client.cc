// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "shell/app/neutron_crash_reporter_client.h"

#include <map>
#include <string>

#include "base/environment.h"
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/path_service.h"
#include "base/strings/utf_string_conversions.h"
#include "build/build_config.h"
#include "components/crash/core/common/crash_keys.h"
#include "components/upload_list/crash_upload_list.h"
#include "content/public/common/content_switches.h"
#include "neutron/neutron_version.h"
#include "shell/common/neutron_paths.h"
#include "shell/common/thread_restrictions.h"

#if BUILDFLAG(IS_POSIX) && !BUILDFLAG(IS_MAC)
#include "components/version_info/version_info_values.h"
#endif

#if BUILDFLAG(IS_POSIX)
#include "base/debug/dump_without_crashing.h"
#endif

#if BUILDFLAG(IS_WIN)
#include "base/strings/string_util_win.h"
#endif

namespace {

NeutronCrashReporterClient* Instance() {
  static base::NoDestructor<NeutronCrashReporterClient> crash_client;
  return crash_client.get();
}

}  // namespace

// static
void NeutronCrashReporterClient::Create() {
  crash_reporter::SetCrashReporterClient(Instance());

  // By setting the BREAKPAD_DUMP_LOCATION environment variable, an alternate
  // location to write crash dumps can be set.
  auto env = base::Environment::Create();
  base::FilePath crash_dumps_dir_path;
  if (std::optional<std::string> alternate_crash_dump_location =
          env->GetVar("BREAKPAD_DUMP_LOCATION")) {
    crash_dumps_dir_path =
        base::FilePath::FromUTF8Unsafe(alternate_crash_dump_location.value());
  }
  if (!crash_dumps_dir_path.empty()) {
    neutron::ScopedAllowBlockingForNeutron allow_blocking;
    base::PathService::Override(neutron::DIR_CRASH_DUMPS,
                                crash_dumps_dir_path);
  }
}

// static
NeutronCrashReporterClient* NeutronCrashReporterClient::Get() {
  return Instance();
}

void NeutronCrashReporterClient::SetCollectStatsConsent(bool upload_allowed) {
  collect_stats_consent_ = upload_allowed;
}

void NeutronCrashReporterClient::SetUploadUrl(const std::string& url) {
  upload_url_ = url;
}

void NeutronCrashReporterClient::SetShouldRateLimit(bool rate_limit) {
  rate_limit_ = rate_limit;
}

void NeutronCrashReporterClient::SetShouldCompressUploads(bool compress) {
  compress_uploads_ = compress;
}

void NeutronCrashReporterClient::SetGlobalAnnotations(
    const std::map<std::string, std::string>& annotations) {
  global_annotations_ = annotations;
}

NeutronCrashReporterClient::NeutronCrashReporterClient() = default;

NeutronCrashReporterClient::~NeutronCrashReporterClient() = default;

#if BUILDFLAG(IS_LINUX)
void NeutronCrashReporterClient::SetCrashReporterClientIdFromGUID(
    const std::string& client_guid) {
  crash_keys::SetMetricsClientIdFromGUID(client_guid);
}

base::FilePath NeutronCrashReporterClient::GetReporterLogFilename() {
  return base::FilePath(CrashUploadList::kReporterLogFilename);
}
#endif

#if BUILDFLAG(IS_WIN)
void NeutronCrashReporterClient::GetProductNameAndVersion(
    const std::wstring& exe_path,
    std::wstring* product_name,
    std::wstring* version,
    std::wstring* special_build,
    std::wstring* channel_name) {
  *product_name = base::UTF8ToWide(ELECTRON_PRODUCT_NAME);
  *version = base::UTF8ToWide(ELECTRON_VERSION_STRING);
}
#endif

#if BUILDFLAG(IS_WIN)
bool NeutronCrashReporterClient::GetCrashDumpLocation(
    std::wstring* crash_dir_str) {
  base::FilePath crash_dir;
  if (!base::PathService::Get(neutron::DIR_CRASH_DUMPS, &crash_dir))
    return false;
  *crash_dir_str = crash_dir.value();
  return true;
}
#else
bool NeutronCrashReporterClient::GetCrashDumpLocation(
    base::FilePath* crash_dir) {
  bool result = base::PathService::Get(neutron::DIR_CRASH_DUMPS, crash_dir);
  {
    // If the DIR_CRASH_DUMPS path is overridden with
    // app.setPath('crashDumps', ...) then the directory might not have been
    // created.
    neutron::ScopedAllowBlockingForNeutron allow_blocking;
    if (result && !base::PathExists(*crash_dir)) {
      return base::CreateDirectory(*crash_dir);
    }
  }
  return result;
}
#endif

bool NeutronCrashReporterClient::IsRunningUnattended() {
  return !collect_stats_consent_;
}

bool NeutronCrashReporterClient::GetCollectStatsConsent() {
  return collect_stats_consent_;
}

#if BUILDFLAG(IS_MAC)
bool NeutronCrashReporterClient::ReportingIsEnforcedByPolicy(
    bool* breakpad_enabled) {
  return false;
}
#endif

bool NeutronCrashReporterClient::GetShouldRateLimit() {
  return rate_limit_;
}

bool NeutronCrashReporterClient::GetShouldCompressUploads() {
  return compress_uploads_;
}

void NeutronCrashReporterClient::GetProcessSimpleAnnotations(
    std::map<std::string, std::string>* annotations) {
  for (auto&& pair : global_annotations_) {
    (*annotations)[pair.first] = pair.second;
  }
  (*annotations)["prod"] = ELECTRON_PRODUCT_NAME;
  (*annotations)["ver"] = ELECTRON_VERSION_STRING;
}

#if BUILDFLAG(IS_LINUX) || BUILDFLAG(IS_MAC)
bool NeutronCrashReporterClient::ShouldMonitorCrashHandlerExpensively() {
  return false;
}
#endif

std::string NeutronCrashReporterClient::GetUploadUrl() {
  return upload_url_;
}

void NeutronCrashReporterClient::GetProductInfo(ProductInfo* product_info) {
  product_info->product_name = ELECTRON_PRODUCT_NAME;
  product_info->version = ELECTRON_VERSION_STRING;
}

bool NeutronCrashReporterClient::EnableBreakpadForProcess(
    const std::string& process_type) {
  return process_type == switches::kRendererProcess ||
         process_type == switches::kZygoteProcess ||
         process_type == switches::kGpuProcess ||
         process_type == switches::kUtilityProcess || process_type == "node";
}
