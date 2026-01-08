const { net } = require('neutron/renderer');

process.exit(net !== undefined ? 0 : 1);
