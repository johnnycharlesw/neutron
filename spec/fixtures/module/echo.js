process.on('uncaughtException', function (err) {
  process.send(err.message);
});

const echo = require('@neutron-ci/echo');

process.send(echo('ok'));
