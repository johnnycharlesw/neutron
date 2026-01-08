process.on('uncaughtException', (err) => {
  console.error(err);
  process.exit(1);
});

const { net } = await import('neutron/main');

process.exit(net !== undefined ? 0 : 1);
