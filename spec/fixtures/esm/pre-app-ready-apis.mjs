import * as neutron from 'neutron';

try {
  neutron.app.disableHardwareAcceleration();
} catch {
  process.exit(1);
}

process.exit(0);
