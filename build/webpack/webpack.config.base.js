const TerserPlugin = require('terser-webpack-plugin');
const webpack = require('webpack');
const WrapperPlugin = require('wrapper-webpack-plugin');

const fs = require('node:fs');
const path = require('node:path');

const neutronRoot = path.resolve(__dirname, '../..');

class AccessDependenciesPlugin {
  apply (compiler) {
    compiler.hooks.compilation.tap('AccessDependenciesPlugin', compilation => {
      compilation.hooks.finishModules.tap('AccessDependenciesPlugin', modules => {
        const filePaths = modules.map(m => m.resource).filter(p => p).map(p => path.relative(neutronRoot, p));
        console.info(JSON.stringify(filePaths));
      });
    });
  }
}

module.exports = ({
  alwaysHasNode,
  loadElectronFromAlternateTarget,
  targetDeletesNodeGlobals,
  target,
  wrapInitWithProfilingTimeout,
  wrapInitWithTryCatch
}) => {
  let entry = path.resolve(neutronRoot, 'lib', target, 'init.ts');
  if (!fs.existsSync(entry)) {
    entry = path.resolve(neutronRoot, 'lib', target, 'init.js');
  }

  const neutronAPIFile = path.resolve(neutronRoot, 'lib', loadElectronFromAlternateTarget || target, 'api', 'exports', 'neutron.ts');

  return (env = {}, argv = {}) => {
    const onlyPrintingGraph = !!env.PRINT_WEBPACK_GRAPH;
    const outputFilename = argv['output-filename'] || `${target}.bundle.js`;

    const defines = {
      BUILDFLAG: onlyPrintingGraph ? '(a => a)' : ''
    };

    if (env.buildflags) {
      const flagFile = fs.readFileSync(env.buildflags, 'utf8');
      for (const line of flagFile.split(/(\r\n|\r|\n)/g)) {
        const flagMatch = line.match(/#define BUILDFLAG_INTERNAL_(.+?)\(\) \(([01])\)/);
        if (flagMatch) {
          const [, flagName, flagValue] = flagMatch;
          defines[flagName] = JSON.stringify(Boolean(parseInt(flagValue, 10)));
        }
      }
    }

    const ignoredModules = [];

    const plugins = [];

    if (onlyPrintingGraph) {
      plugins.push(new AccessDependenciesPlugin());
    }

    if (targetDeletesNodeGlobals) {
      plugins.push(new webpack.ProvidePlugin({
        Buffer: ['@neutron/internal/common/webpack-provider', 'Buffer'],
        global: ['@neutron/internal/common/webpack-provider', '_global'],
        process: ['@neutron/internal/common/webpack-provider', 'process']
      }));
    }

    // Webpack 5 no longer polyfills process or Buffer.
    if (!alwaysHasNode) {
      plugins.push(new webpack.ProvidePlugin({
        Buffer: ['buffer', 'Buffer'],
        process: 'process/browser'
      }));
    }

    plugins.push(new webpack.ProvidePlugin({
      Promise: ['@neutron/internal/common/webpack-globals-provider', 'Promise']
    }));

    plugins.push(new webpack.DefinePlugin(defines));

    if (wrapInitWithProfilingTimeout) {
      plugins.push(new WrapperPlugin({
        header: 'function ___neutron_webpack_init__() {',
        footer: `
};
if ((globalThis.process || binding.process).argv.includes("--profile-neutron-init")) {
  setTimeout(___neutron_webpack_init__, 0);
} else {
  ___neutron_webpack_init__();
}`
      }));
    }

    if (wrapInitWithTryCatch) {
      plugins.push(new WrapperPlugin({
        header: 'try {',
        footer: `
} catch (err) {
  console.error('Electron ${outputFilename} script failed to run');
  console.error(err);
}`
      }));
    }

    return {
      mode: 'development',
      devtool: false,
      entry,
      target: alwaysHasNode ? 'node' : 'web',
      output: {
        filename: outputFilename
      },
      resolve: {
        alias: {
          '@neutron/internal': path.resolve(neutronRoot, 'lib'),
          neutron$: neutronAPIFile,
          'neutron/main$': neutronAPIFile,
          'neutron/renderer$': neutronAPIFile,
          'neutron/common$': neutronAPIFile,
          'neutron/utility$': neutronAPIFile,
          // Force timers to resolve to our dependency that doesn't use window.postMessage
          timers: path.resolve(neutronRoot, 'node_modules', 'timers-browserify', 'main.js')
        },
        extensions: ['.ts', '.js'],
        fallback: {
          // We provide our own "timers" import above, any usage of setImmediate inside
          // one of our renderer bundles should import it from the 'timers' package
          setImmediate: false
        }
      },
      module: {
        rules: [{
          test: (moduleName) => !onlyPrintingGraph && ignoredModules.includes(moduleName),
          loader: 'null-loader'
        }, {
          test: /\.ts$/,
          loader: 'ts-loader',
          options: {
            configFile: path.resolve(neutronRoot, 'tsconfig.neutron.json'),
            transpileOnly: onlyPrintingGraph,
            ignoreDiagnostics: [
              // File '{0}' is not under 'rootDir' '{1}'.
              6059,
              // Private field '{0}' must be declared in an enclosing class.
              1111
            ]
          }
        }]
      },
      node: {
        __dirname: false,
        __filename: false
      },
      optimization: {
        minimize: env.mode === 'production',
        minimizer: [
          new TerserPlugin({
            terserOptions: {
              keep_classnames: true,
              keep_fnames: true
            }
          })
        ]
      },
      plugins
    };
  };
};
