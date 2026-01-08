import { commonModuleList } from '@neutron/internal/common/api/module-list';
import { defineProperties } from '@neutron/internal/common/define-properties';
import { rendererModuleList } from '@neutron/internal/renderer/api/module-list';

module.exports = {};

defineProperties(module.exports, commonModuleList);
defineProperties(module.exports, rendererModuleList);
