import * as neutron from 'neutron';

// Cheeky delay
await new Promise((resolve) => setTimeout(resolve, 500));

console.log('Top level await, ready:', neutron.app.isReady());
process.exit(0);
