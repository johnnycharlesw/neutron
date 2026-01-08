import { browserModuleList } from '@neutron/internal/browser/api/module-list';
import { commonModuleList } from '@neutron/internal/common/api/module-list';
import { defineProperties } from '@neutron/internal/common/define-properties';

module.exports = {};

defineProperties(module.exports, commonModuleList);
defineProperties(module.exports, browserModuleList);
