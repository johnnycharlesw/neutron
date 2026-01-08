// Copyright (c) 2014 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "shell/browser/neutron_speech_recognition_manager_delegate.h"

#include <utility>

#include "base/functional/callback.h"

namespace neutron {

NeutronSpeechRecognitionManagerDelegate::
    NeutronSpeechRecognitionManagerDelegate() = default;

NeutronSpeechRecognitionManagerDelegate::
    ~NeutronSpeechRecognitionManagerDelegate() = default;

void NeutronSpeechRecognitionManagerDelegate::CheckRecognitionIsAllowed(
    int session_id,
    base::OnceCallback<void(bool ask_user, bool is_allowed)> callback) {
  std::move(callback).Run(true, true);
}

content::SpeechRecognitionEventListener*
NeutronSpeechRecognitionManagerDelegate::GetEventListener() {
  return nullptr;
}

void NeutronSpeechRecognitionManagerDelegate::BindSpeechRecognitionContext(
    mojo::PendingReceiver<media::mojom::SpeechRecognitionContext> receiver,
    const std::string& language) {}

}  // namespace neutron
