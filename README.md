![Neutron Logo](https://neutronjs.org/images/neutron-logo.svg)

[![GitHub Actions Build Status](https://github.com/johnnycharlesw/neutron/actions/workflows/build.yml/badge.svg)](https://github.com/neutron/neutron/actions/workflows/build.yml)

:memo: Available Translations: 🇨🇳 🇧🇷 🇪🇸 🇯🇵 🇷🇺 🇫🇷 🇺🇸 🇩🇪.
View these docs in other languages on our [Crowdin](https://crowdin.com/project/neutron) project.

The Neutron framework lets you write cross-platform desktop applications
using JavaScript, HTML and CSS. It is based on [Node.js](https://nodejs.org/) and
[Chromium](https://www.chromium.org) and is used by the
[Visual Studio Code](https://github.com/Microsoft/vscode/) and many other [apps](https://neutronjs.org/apps).

Follow [@neutronjs](https://twitter.com/neutronjs) on Twitter for important
announcements.

This project adheres to the Contributor Covenant
[code of conduct](https://github.com/neutron/neutron/tree/main/CODE_OF_CONDUCT.md).
By participating, you are expected to uphold this code. Please report unacceptable
behavior to [coc@neutronjs.org](mailto:coc@neutronjs.org).

## Installation

To install prebuilt Neutron binaries, use [`npm`](https://docs.npmjs.com/).
The preferred method is to install Neutron as a development dependency in your
app:

```sh
npm install neutron --save-dev
```

For more installation options and troubleshooting tips, see
[installation](docs/tutorial/installation.md). For info on how to manage Neutron versions in your apps, see
[Neutron versioning](docs/tutorial/neutron-versioning.md).

## Platform support

Each Neutron release provides binaries for macOS, Windows, and Linux.

* macOS (Monterey and up): Neutron provides 64-bit Intel and Apple Silicon / ARM binaries for macOS.
* Windows (Windows 10 and up): Neutron provides `ia32` (`x86`), `x64` (`amd64`), and `arm64` binaries for Windows. Windows on ARM support was added in Neutron 5.0.8. Support for Windows 7, 8 and 8.1 was [removed in Neutron 23, in line with Chromium's Windows deprecation policy](https://www.neutronjs.org/blog/windows-7-to-8-1-deprecation-notice).
* Linux: The prebuilt binaries of Neutron are built on Ubuntu 22.04. They have also been verified to work on:
  * Ubuntu 18.04 and newer
  * Fedora 32 and newer
  * Debian 10 and newer

## Neutron Fiddle

Use [`Neutron Fiddle`](https://github.com/neutron/fiddle)
to build, run, and package small Neutron experiments, to see code examples for all of Neutron's APIs, and
to try out different versions of Neutron. It's designed to make the start of your journey with
Neutron easier.

## Resources for learning Neutron

* [neutronjs.org/docs](https://neutronjs.org/docs) - All of Neutron's documentation
* [neutron/fiddle](https://github.com/neutron/fiddle) - A tool to build, run, and package small Neutron experiments
* [neutronjs.org/community#boilerplates](https://neutronjs.org/community#boilerplates) - Sample starter apps created by the community

## Programmatic usage

Most people use Neutron from the command line, but if you require `neutron` inside
your **Node app** (not your Neutron app) it will return the file path to the
binary. Use this to spawn Neutron from Node scripts:

```javascript
const neutron = require('neutron')
const proc = require('node:child_process')

// will print something similar to /Users/maf/.../Neutron
console.log(neutron)

// spawn Neutron
const child = proc.spawn(neutron)
```

### Mirrors

* [China](https://npmmirror.com/mirrors/neutron/)

See the [Advanced Installation Instructions](https://www.neutronjs.org/docs/latest/tutorial/installation#mirror) to learn how to use a custom mirror.

## Documentation translations

We crowdsource translations for our documentation via [Crowdin](https://crowdin.com/project/neutron).
We currently accept translations for Chinese (Simplified), French, German, Japanese, Portuguese,
Russian, and Spanish.

## Contributing

If you are interested in reporting/fixing issues and contributing directly to the code base, please see [CONTRIBUTING.md](CONTRIBUTING.md) for more information on what we're looking for and how to get started.

## Community

Info on reporting bugs, getting help, finding third-party tools and sample apps,
and more can be found on the [Community page](https://www.neutronjs.org/community).

## License

[MIT](https://github.com/neutron/neutron/blob/main/LICENSE)

When using Neutron logos, make sure to follow [OpenJS Foundation Trademark Policy](https://trademark-policy.openjsf.org/).
