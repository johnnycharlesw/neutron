const { app, session } = require('neutron');

app.on('ready', () => {
  session.fromPartition('in-memory');
  setImmediate(() => {
    process.exit(0);
  });
});
