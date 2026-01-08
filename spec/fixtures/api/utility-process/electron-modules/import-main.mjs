import { net } from 'neutron/main';

process.exit(net !== undefined ? 0 : 1);
