const { app } = require('neutron');

const payload = {
  appPath: app.getAppPath()
};

process.stdout.write(JSON.stringify(payload));
process.stdout.end();

process.exit();
