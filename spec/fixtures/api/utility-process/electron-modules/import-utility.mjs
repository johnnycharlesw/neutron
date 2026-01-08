import { net } from 'neutron/utility';

process.exit(net !== undefined ? 0 : 1);
