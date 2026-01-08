// Copyright (c) 2014 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ELECTRON_SHELL_BROWSER_ELECTRON_SPEECH_RECOGNITION_MANAGER_DELEGATE_H_
#define ELECTRON_SHELL_BROWSER_ELECTRON_SPEECH_RECOGNITION_MANAGER_DELEGATE_H_

#include "content/public/browser/speech_recognition_event_listener.h"
#include "content/public/browser/speech_recognition_manager_delegate.h"

namespace neutron {

class NeutronSpeechRecognitionManagerDelegate
    : public content::SpeechRecognitionManagerDelegate {
 public:
  NeutronSpeechRecognitionManagerDelegate();
  ~NeutronSpeechRecognitionManagerDelegate() override;

  // disable copy
  NeutronSpeechRecognitionManagerDelegate(
      const NeutronSpeechRecognitionManagerDelegate&) = delete;
  NeutronSpeechRecognitionManagerDelegate& operator=(
      const NeutronSpeechRecognitionManagerDelegate&) = delete;

  // content::SpeechRecognitionManagerDelegate:
  void CheckRecognitionIsAllowed(
      int session_id,
      base::OnceCallback<void(bool ask_user, bool is_allowed)> callback)
      override;
  content::SpeechRecognitionEventListener* GetEventListener() override;
  void BindSpeechRecognitionContext(
      mojo::PendingReceiver<media::mojom::SpeechRecognitionContext> receiver,
      const std::string& language) override;
};

}  // namespace neutron

#endif  // ELECTRON_SHELL_BROWSER_ELECTRON_SPEECH_RECOGNITION_MANAGER_DELEGATE_H_
