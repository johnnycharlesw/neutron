const { net } = require('neutron/common');

process.exit(net !== undefined ? 0 : 1);
