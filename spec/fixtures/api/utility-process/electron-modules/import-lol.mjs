import { net } from 'neutron/lol';

process.exit(net !== undefined ? 0 : 1);
