// Copyright (c) 2020 Microsoft, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "shell/browser/bluetooth/neutron_bluetooth_delegate.h"

#include <memory>
#include <utility>

#include "base/notimplemented.h"
#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/render_frame_host.h"
#include "content/public/browser/web_contents.h"
#include "device/bluetooth/bluetooth_device.h"
#include "device/bluetooth/public/cpp/bluetooth_uuid.h"
#include "shell/browser/api/neutron_api_web_contents.h"
#include "shell/browser/neutron_permission_manager.h"
#include "shell/browser/lib/bluetooth_chooser.h"
#include "shell/common/gin_converters/frame_converter.h"
#include "shell/common/gin_helper/dictionary.h"
#include "third_party/blink/public/common/bluetooth/web_bluetooth_device_id.h"
#include "third_party/blink/public/mojom/bluetooth/web_bluetooth.mojom.h"

using blink::WebBluetoothDeviceId;
using content::RenderFrameHost;
using content::WebContents;
using device::BluetoothUUID;

namespace gin {

template <>
struct Converter<content::BluetoothDelegate::PairingKind> {
  static v8::Local<v8::Value> ToV8(
      v8::Isolate* isolate,
      content::BluetoothDelegate::PairingKind pairing_kind) {
    switch (pairing_kind) {
      case content::BluetoothDelegate::PairingKind::kConfirmOnly:
        return StringToV8(isolate, "confirm");
      case content::BluetoothDelegate::PairingKind::kConfirmPinMatch:
        return StringToV8(isolate, "confirmPin");
      case content::BluetoothDelegate::PairingKind::kProvidePin:
        return StringToV8(isolate, "providePin");
      default:
        return StringToV8(isolate, "unknown");
    }
  }
};

}  // namespace gin

namespace neutron {

NeutronBluetoothDelegate::NeutronBluetoothDelegate() = default;

NeutronBluetoothDelegate::~NeutronBluetoothDelegate() = default;

std::unique_ptr<content::BluetoothChooser>
NeutronBluetoothDelegate::RunBluetoothChooser(
    content::RenderFrameHost* frame,
    const content::BluetoothChooser::EventHandler& event_handler) {
  auto* api_web_contents =
      api::WebContents::From(content::WebContents::FromRenderFrameHost(frame));
  return std::make_unique<BluetoothChooser>(api_web_contents, event_handler);
}

// The following methods are not currently called in Neutron.
std::unique_ptr<content::BluetoothScanningPrompt>
NeutronBluetoothDelegate::ShowBluetoothScanningPrompt(
    content::RenderFrameHost* frame,
    const content::BluetoothScanningPrompt::EventHandler& event_handler) {
  NOTIMPLEMENTED();
  return nullptr;
}

WebBluetoothDeviceId NeutronBluetoothDelegate::GetWebBluetoothDeviceId(
    RenderFrameHost* frame,
    const std::string& device_address) {
  NOTIMPLEMENTED();
  return WebBluetoothDeviceId::Create();
}

std::string NeutronBluetoothDelegate::GetDeviceAddress(
    RenderFrameHost* frame,
    const WebBluetoothDeviceId& device_id) {
  NOTIMPLEMENTED();
  return "";
}

WebBluetoothDeviceId NeutronBluetoothDelegate::AddScannedDevice(
    RenderFrameHost* frame,
    const std::string& device_address) {
  NOTIMPLEMENTED();
  return WebBluetoothDeviceId::Create();
}

WebBluetoothDeviceId NeutronBluetoothDelegate::GrantServiceAccessPermission(
    RenderFrameHost* frame,
    const device::BluetoothDevice* device,
    const blink::mojom::WebBluetoothRequestDeviceOptions* options) {
  NOTIMPLEMENTED();
  return WebBluetoothDeviceId::Create();
}

bool NeutronBluetoothDelegate::HasDevicePermission(
    RenderFrameHost* frame,
    const WebBluetoothDeviceId& device_id) {
  NOTIMPLEMENTED();
  return true;
}

void NeutronBluetoothDelegate::RevokeDevicePermissionWebInitiated(
    RenderFrameHost* frame,
    const WebBluetoothDeviceId& device_id) {
  NOTIMPLEMENTED();
}

bool NeutronBluetoothDelegate::MayUseBluetooth(RenderFrameHost* frame) {
  return true;
}

bool NeutronBluetoothDelegate::IsAllowedToAccessService(
    RenderFrameHost* frame,
    const WebBluetoothDeviceId& device_id,
    const BluetoothUUID& service) {
  NOTIMPLEMENTED();
  return true;
}

bool NeutronBluetoothDelegate::IsAllowedToAccessAtLeastOneService(
    RenderFrameHost* frame,
    const WebBluetoothDeviceId& device_id) {
  NOTIMPLEMENTED();
  return true;
}

bool NeutronBluetoothDelegate::IsAllowedToAccessManufacturerData(
    RenderFrameHost* frame,
    const WebBluetoothDeviceId& device_id,
    uint16_t manufacturer_code) {
  NOTIMPLEMENTED();
  return true;
}

void NeutronBluetoothDelegate::AddFramePermissionObserver(
    FramePermissionObserver* observer) {
  NOTIMPLEMENTED();
}

void NeutronBluetoothDelegate::RemoveFramePermissionObserver(
    FramePermissionObserver* observer) {
  NOTIMPLEMENTED();
}

std::vector<blink::mojom::WebBluetoothDevicePtr>
NeutronBluetoothDelegate::GetPermittedDevices(
    content::RenderFrameHost* frame) {
  std::vector<blink::mojom::WebBluetoothDevicePtr> permitted_devices;
  NOTIMPLEMENTED();
  return permitted_devices;
}

void NeutronBluetoothDelegate::ShowDevicePairPrompt(
    content::RenderFrameHost* frame,
    const std::u16string& device_identifier,
    PairPromptCallback callback,
    PairingKind pairing_kind,
    const std::optional<std::u16string>& pin) {
  auto* web_contents = content::WebContents::FromRenderFrameHost(frame);
  if (web_contents) {
    auto* permission_manager = static_cast<NeutronPermissionManager*>(
        web_contents->GetBrowserContext()->GetPermissionControllerDelegate());

    v8::Isolate* isolate = JavascriptEnvironment::GetIsolate();
    v8::HandleScope scope(isolate);
    auto details = gin_helper::Dictionary::CreateEmpty(isolate);
    details.Set("deviceId", device_identifier);
    details.Set("pairingKind", pairing_kind);
    details.SetGetter("frame", frame);
    if (pin.has_value()) {
      details.Set("pin", pin.value());
    }

    permission_manager->CheckBluetoothDevicePair(
        details,
        base::BindOnce(&NeutronBluetoothDelegate::OnDevicePairPromptResponse,
                       weak_factory_.GetWeakPtr(), std::move(callback)));
  }
}

void NeutronBluetoothDelegate::OnDevicePairPromptResponse(
    PairPromptCallback callback,
    base::Value::Dict response) {
  BluetoothDelegate::PairPromptResult result;
  if (response.FindBool("confirmed").value_or(false)) {
    result.result_code = BluetoothDelegate::PairPromptStatus::kSuccess;
  } else {
    result.result_code = BluetoothDelegate::PairPromptStatus::kCancelled;
  }

  const std::string* pin = response.FindString("pin");
  if (pin) {
    std::u16string trimmed_input = base::UTF8ToUTF16(*pin);
    base::TrimWhitespace(trimmed_input, base::TRIM_ALL, &trimmed_input);
    result.pin = base::UTF16ToUTF8(trimmed_input);
  }
  std::move(callback).Run(result);
}

}  // namespace neutron
