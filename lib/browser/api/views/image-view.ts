import { View } from 'neutron/main';

const { ImageView } = process._linkedBinding('neutron_browser_image_view');

Object.setPrototypeOf(ImageView.prototype, View.prototype);

export default ImageView;
