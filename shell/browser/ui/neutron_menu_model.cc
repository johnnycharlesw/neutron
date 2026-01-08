// Copyright (c) 2015 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "shell/browser/ui/neutron_menu_model.h"

#include <utility>

namespace neutron {

#if BUILDFLAG(IS_MAC)
NeutronMenuModel::SharingItem::SharingItem() = default;
NeutronMenuModel::SharingItem::SharingItem(SharingItem&&) = default;
NeutronMenuModel::SharingItem::~SharingItem() = default;
#endif

bool NeutronMenuModel::Delegate::GetAcceleratorForCommandId(
    int command_id,
    ui::Accelerator* accelerator) const {
  return GetAcceleratorForCommandIdWithParams(command_id, false, accelerator);
}

NeutronMenuModel::NeutronMenuModel(Delegate* delegate)
    : ui::SimpleMenuModel(delegate), delegate_(delegate) {}

NeutronMenuModel::~NeutronMenuModel() = default;

void NeutronMenuModel::SetToolTip(size_t index,
                                   const std::u16string& toolTip) {
  int command_id = GetCommandIdAt(index);
  toolTips_[command_id] = toolTip;
}

std::u16string NeutronMenuModel::GetToolTipAt(size_t index) {
  const int command_id = GetCommandIdAt(index);
  const auto iter = toolTips_.find(command_id);
  return iter == std::end(toolTips_) ? std::u16string() : iter->second;
}

void NeutronMenuModel::SetCustomType(size_t index,
                                      const std::u16string& customType) {
  int command_id = GetCommandIdAt(index);
  customTypes_[command_id] = customType;
}

std::u16string NeutronMenuModel::GetCustomTypeAt(size_t index) {
  const int command_id = GetCommandIdAt(index);
  const auto iter = customTypes_.find(command_id);
  return iter == std::end(customTypes_) ? std::u16string() : iter->second;
}

void NeutronMenuModel::SetRole(size_t index, const std::u16string& role) {
  int command_id = GetCommandIdAt(index);
  roles_[command_id] = role;
}

std::u16string NeutronMenuModel::GetRoleAt(size_t index) {
  const int command_id = GetCommandIdAt(index);
  const auto iter = roles_.find(command_id);
  return iter == std::end(roles_) ? std::u16string() : iter->second;
}

void NeutronMenuModel::SetSecondaryLabel(size_t index,
                                          const std::u16string& sublabel) {
  int command_id = GetCommandIdAt(index);
  sublabels_[command_id] = sublabel;
}

std::u16string NeutronMenuModel::GetSecondaryLabelAt(size_t index) const {
  int command_id = GetCommandIdAt(index);
  const auto iter = sublabels_.find(command_id);
  return iter == std::end(sublabels_) ? std::u16string() : iter->second;
}

bool NeutronMenuModel::GetAcceleratorAtWithParams(
    size_t index,
    bool use_default_accelerator,
    ui::Accelerator* accelerator) const {
  if (delegate_) {
    return delegate_->GetAcceleratorForCommandIdWithParams(
        GetCommandIdAt(index), use_default_accelerator, accelerator);
  }
  return false;
}

bool NeutronMenuModel::ShouldRegisterAcceleratorAt(size_t index) const {
  if (delegate_) {
    return delegate_->ShouldRegisterAcceleratorForCommandId(
        GetCommandIdAt(index));
  }
  return true;
}

bool NeutronMenuModel::WorksWhenHiddenAt(size_t index) const {
  if (delegate_) {
    return delegate_->ShouldCommandIdWorkWhenHidden(GetCommandIdAt(index));
  }
  return true;
}

#if BUILDFLAG(IS_MAC)
bool NeutronMenuModel::GetSharingItemAt(size_t index,
                                         SharingItem* item) const {
  if (delegate_)
    return delegate_->GetSharingItemForCommandId(GetCommandIdAt(index), item);
  return false;
}

void NeutronMenuModel::SetSharingItem(SharingItem item) {
  sharing_item_.emplace(std::move(item));
}
#endif

void NeutronMenuModel::MenuWillClose() {
  ui::SimpleMenuModel::MenuWillClose();
  observers_.Notify(&Observer::OnMenuWillClose);
}

void NeutronMenuModel::MenuWillShow() {
  ui::SimpleMenuModel::MenuWillShow();
  observers_.Notify(&Observer::OnMenuWillShow);
}

NeutronMenuModel* NeutronMenuModel::GetSubmenuModelAt(size_t index) {
  return static_cast<NeutronMenuModel*>(
      ui::SimpleMenuModel::GetSubmenuModelAt(index));
}

}  // namespace neutron
