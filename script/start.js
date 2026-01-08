const cp = require('node:child_process');

const utils = require('./lib/utils');

const neutronPath = utils.getAbsoluteElectronExec();

const child = cp.spawn(neutronPath, process.argv.slice(2), { stdio: 'inherit' });
let childClosed = false;
child.on('close', (code) => {
  childClosed = true;
  process.exit(code);
});

const handleTerminationSignal = (signal) =>
  process.on(signal, () => {
    if (!childClosed) {
      child.kill(signal);
    }
  });

handleTerminationSignal('SIGINT');
handleTerminationSignal('SIGTERM');
handleTerminationSignal('SIGUSR2');
