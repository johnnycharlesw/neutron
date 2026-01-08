const binding = process._linkedBinding('neutron_renderer_web_utils');

export const getPathForFile = binding.getPathForFile;
