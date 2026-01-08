const { net } = require('neutron/utility');

process.exit(net !== undefined ? 0 : 1);
