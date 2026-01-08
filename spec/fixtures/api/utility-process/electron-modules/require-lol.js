const { net } = require('neutron/lol');

process.exit(net !== undefined ? 0 : 1);
