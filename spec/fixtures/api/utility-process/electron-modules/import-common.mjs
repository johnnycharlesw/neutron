import { net } from 'neutron/common';

process.exit(net !== undefined ? 0 : 1);
