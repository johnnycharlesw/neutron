const { net } = require('neutron/main');

process.exit(net !== undefined ? 0 : 1);
