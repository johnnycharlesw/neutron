import { net } from 'neutron/renderer';

process.exit(net !== undefined ? 0 : 1);
