let echo;
try {
  echo = require('@neutron-ci/echo');
} catch {
  process.exit(1);
}
process.exit(echo(0));
