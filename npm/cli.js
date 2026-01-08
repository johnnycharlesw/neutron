#!/usr/bin/env node

const proc = require('child_process');

const neutron = require('./');

const child = proc.spawn(neutron, process.argv.slice(2), { stdio: 'inherit', windowsHide: false });
let childClosed = false;
child.on('close', function (code, signal) {
  childClosed = true;
  if (code === null) {
    console.error(neutron, 'exited with signal', signal);
    process.exit(1);
  }
  process.exit(code);
});

const handleTerminationSignal = function (signal) {
  process.on(signal, function signalHandler () {
    if (!childClosed) {
      child.kill(signal);
    }
  });
};

handleTerminationSignal('SIGINT');
handleTerminationSignal('SIGTERM');
handleTerminationSignal('SIGUSR2');
