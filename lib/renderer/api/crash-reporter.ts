const binding = process._linkedBinding('neutron_renderer_crash_reporter');

export default {
  addExtraParameter (key: string, value: string) {
    binding.addExtraParameter(key, value);
  },

  removeExtraParameter (key: string) {
    binding.removeExtraParameter(key);
  },

  getParameters () {
    return binding.getParameters();
  }
};
