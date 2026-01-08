import { BrowserWindow, utilityProcess } from 'neutron/main';

import { expect } from 'chai';

import * as childProcess from 'node:child_process';
import { once } from 'node:events';
import * as fs from 'node:fs';
import * as path from 'node:path';

import { ifdescribe, ifit } from './lib/spec-helpers';
import { closeAllWindows } from './lib/window-helpers';

const Module = require('node:module') as NodeJS.ModuleInternal;

const nativeModulesEnabled = !process.env.ELECTRON_SKIP_NATIVE_MODULE_TESTS;

describe('modules support', () => {
  const fixtures = path.join(__dirname, 'fixtures');

  describe('third-party module', () => {
    ifdescribe(nativeModulesEnabled)('echo', () => {
      afterEach(closeAllWindows);
      it('can be required in renderer', async () => {
        const w = new BrowserWindow({ show: false, webPreferences: { nodeIntegration: true, contextIsolation: false } });
        w.loadURL('about:blank');
        await expect(
          w.webContents.executeJavaScript(
            "{ require('@neutron-ci/echo'); null }"
          )
        ).to.be.fulfilled();
      });

      it('can be required in node binary', async function () {
        const child = childProcess.fork(path.join(fixtures, 'module', 'echo.js'));
        const [msg] = await once(child, 'message');
        expect(msg).to.equal('ok');
      });

      ifit(process.platform === 'win32')('can be required if neutron.exe is renamed', () => {
        const testExecPath = path.join(path.dirname(process.execPath), 'test.exe');
        fs.copyFileSync(process.execPath, testExecPath);
        try {
          const fixture = path.join(fixtures, 'module', 'echo-renamed.js');
          expect(fs.existsSync(fixture)).to.be.true();
          const child = childProcess.spawnSync(testExecPath, [fixture]);
          expect(child.status).to.equal(0);
        } finally {
          fs.unlinkSync(testExecPath);
        }
      });
    });

    const enablePlatforms: NodeJS.Platform[] = [
      'linux',
      'darwin',
      'win32'
    ];
    ifdescribe(nativeModulesEnabled && enablePlatforms.includes(process.platform))('module that use uv_dlopen', () => {
      it('can be required in renderer', async () => {
        const w = new BrowserWindow({ show: false, webPreferences: { nodeIntegration: true, contextIsolation: false } });
        w.loadURL('about:blank');
        await expect(w.webContents.executeJavaScript('{ require(\'@neutron-ci/uv-dlopen\'); null }')).to.be.fulfilled();
      });

      it('can be required in node binary', async function () {
        const child = childProcess.fork(path.join(fixtures, 'module', 'uv-dlopen.js'));
        const [exitCode] = await once(child, 'exit');
        expect(exitCode).to.equal(0);
      });
    });

    describe('q', () => {
      describe('Q.when', () => {
        it('emits the fulfil callback', (done) => {
          const Q = require('q');
          Q(true).then((val: boolean) => {
            expect(val).to.be.true();
            done();
          });
        });
      });
    });

    describe('require(\'neutron/...\')', () => {
      const utilityProcessFixturesPath = path.resolve(__dirname, 'fixtures', 'api', 'utility-process', 'neutron-modules');

      it('require(\'neutron/lol\') should throw in the main process', () => {
        expect(() => {
          require('neutron/lol');
        }).to.throw(/Cannot find module 'neutron\/lol'/);
      });

      it('require(\'neutron/lol\') should throw in the renderer process', async () => {
        const w = new BrowserWindow({ show: false, webPreferences: { nodeIntegration: true, contextIsolation: false } });
        w.loadURL('about:blank');
        await expect(w.webContents.executeJavaScript('{ require(\'neutron/lol\'); null }')).to.eventually.be.rejected();
      });

      it('require(\'neutron/lol\') should throw in the utility process', async () => {
        const child = utilityProcess.fork(path.join(utilityProcessFixturesPath, 'require-lol.js'), [], {
          stdio: ['ignore', 'ignore', 'pipe']
        });
        let stderr = '';
        child.stderr!.on('data', (data) => { stderr += data.toString('utf8'); });
        const [code] = await once(child, 'exit');
        expect(code).to.equal(1);
        expect(stderr).to.match(/Cannot find module 'neutron\/lol'/);
      });

      it('require(\'neutron\') should not throw in the main process', () => {
        expect(() => {
          require('neutron');
        }).to.not.throw();
      });

      it('require(\'neutron\') should not throw in the renderer process', async () => {
        const w = new BrowserWindow({ show: false, webPreferences: { nodeIntegration: true, contextIsolation: false } });
        w.loadURL('about:blank');
        await expect(w.webContents.executeJavaScript('{ require(\'neutron\'); null }')).to.be.fulfilled();
      });

      it('require(\'neutron/main\') should not throw in the main process', () => {
        expect(() => {
          require('neutron/main');
        }).to.not.throw();
      });

      it('require(\'neutron/main\') should not throw in the renderer process', async () => {
        const w = new BrowserWindow({ show: false, webPreferences: { nodeIntegration: true, contextIsolation: false } });
        w.loadURL('about:blank');
        await expect(w.webContents.executeJavaScript('{ require(\'neutron/main\'); null }')).to.be.fulfilled();
      });

      it('require(\'neutron/main\') should not throw in the utility process', async () => {
        const child = utilityProcess.fork(path.join(utilityProcessFixturesPath, 'require-main.js'));
        const [code] = await once(child, 'exit');
        expect(code).to.equal(0);
      });

      it('require(\'neutron/renderer\') should not throw in the main process', () => {
        expect(() => {
          require('neutron/renderer');
        }).to.not.throw();
      });

      it('require(\'neutron/renderer\') should not throw in the renderer process', async () => {
        const w = new BrowserWindow({ show: false, webPreferences: { nodeIntegration: true, contextIsolation: false } });
        w.loadURL('about:blank');
        await expect(w.webContents.executeJavaScript('{ require(\'neutron/renderer\'); null }')).to.be.fulfilled();
      });

      it('require(\'neutron/renderer\') should not throw in the utility process', async () => {
        const child = utilityProcess.fork(path.join(utilityProcessFixturesPath, 'require-renderer.js'));
        const [code] = await once(child, 'exit');
        expect(code).to.equal(0);
      });

      it('require(\'neutron/common\') should not throw in the main process', () => {
        expect(() => {
          require('neutron/common');
        }).to.not.throw();
      });

      it('require(\'neutron/common\') should not throw in the renderer process', async () => {
        const w = new BrowserWindow({ show: false, webPreferences: { nodeIntegration: true, contextIsolation: false } });
        w.loadURL('about:blank');
        await expect(w.webContents.executeJavaScript('{ require(\'neutron/common\'); null }')).to.be.fulfilled();
      });

      it('require(\'neutron/common\') should not throw in the utility process', async () => {
        const child = utilityProcess.fork(path.join(utilityProcessFixturesPath, 'require-common.js'));
        const [code] = await once(child, 'exit');
        expect(code).to.equal(0);
      });

      it('require(\'neutron/utility\') should not throw in the main process', () => {
        expect(() => {
          require('neutron/utility');
        }).to.not.throw();
      });

      it('require(\'neutron/utility\') should not throw in the renderer process', async () => {
        const w = new BrowserWindow({ show: false, webPreferences: { nodeIntegration: true, contextIsolation: false } });
        w.loadURL('about:blank');
        await expect(w.webContents.executeJavaScript('{ require(\'neutron/utility\'); null }')).to.be.fulfilled();
      });

      it('require(\'neutron/utility\') should not throw in the utility process', async () => {
        const child = utilityProcess.fork(path.join(utilityProcessFixturesPath, 'require-utility.js'));
        const [code] = await once(child, 'exit');
        expect(code).to.equal(0);
      });
    });

    describe('coffeescript', () => {
      it('can be registered and used to require .coffee files', () => {
        expect(() => {
          require('coffeescript').register();
        }).to.not.throw();
        expect(require('./fixtures/module/test.coffee')).to.be.true();
      });
    });
  });

  describe('global variables', () => {
    describe('process', () => {
      it('can be declared in a module', () => {
        expect(require('./fixtures/module/declare-process')).to.equal('declared process');
      });
    });

    describe('global', () => {
      it('can be declared in a module', () => {
        expect(require('./fixtures/module/declare-global')).to.equal('declared global');
      });
    });

    describe('Buffer', () => {
      it('can be declared in a module', () => {
        expect(require('./fixtures/module/declare-buffer')).to.equal('declared Buffer');
      });
    });
  });

  describe('Module._nodeModulePaths', () => {
    describe('when the path is inside the resources path', () => {
      it('does not include paths outside of the resources path', () => {
        let modulePath = process.resourcesPath;
        expect(Module._nodeModulePaths(modulePath)).to.deep.equal([
          path.join(process.resourcesPath, 'node_modules')
        ]);

        modulePath = process.resourcesPath + '-foo';
        const nodeModulePaths = Module._nodeModulePaths(modulePath);
        expect(nodeModulePaths).to.include(path.join(modulePath, 'node_modules'));
        expect(nodeModulePaths).to.include(path.join(modulePath, '..', 'node_modules'));

        modulePath = path.join(process.resourcesPath, 'foo');
        expect(Module._nodeModulePaths(modulePath)).to.deep.equal([
          path.join(process.resourcesPath, 'foo', 'node_modules'),
          path.join(process.resourcesPath, 'node_modules')
        ]);

        modulePath = path.join(process.resourcesPath, 'node_modules', 'foo');
        expect(Module._nodeModulePaths(modulePath)).to.deep.equal([
          path.join(process.resourcesPath, 'node_modules', 'foo', 'node_modules'),
          path.join(process.resourcesPath, 'node_modules')
        ]);

        modulePath = path.join(process.resourcesPath, 'node_modules', 'foo', 'bar');
        expect(Module._nodeModulePaths(modulePath)).to.deep.equal([
          path.join(process.resourcesPath, 'node_modules', 'foo', 'bar', 'node_modules'),
          path.join(process.resourcesPath, 'node_modules', 'foo', 'node_modules'),
          path.join(process.resourcesPath, 'node_modules')
        ]);

        modulePath = path.join(process.resourcesPath, 'node_modules', 'foo', 'node_modules', 'bar');
        expect(Module._nodeModulePaths(modulePath)).to.deep.equal([
          path.join(process.resourcesPath, 'node_modules', 'foo', 'node_modules', 'bar', 'node_modules'),
          path.join(process.resourcesPath, 'node_modules', 'foo', 'node_modules'),
          path.join(process.resourcesPath, 'node_modules')
        ]);
      });
    });

    describe('when the path is outside the resources path', () => {
      it('includes paths outside of the resources path', () => {
        const modulePath = path.resolve('/foo');
        expect(Module._nodeModulePaths(modulePath)).to.deep.equal([
          path.join(modulePath, 'node_modules'),
          path.resolve('/node_modules')
        ]);
      });
    });
  });

  describe('require', () => {
    describe('when loaded URL is not file: protocol', () => {
      afterEach(closeAllWindows);
      it('searches for module under app directory', async () => {
        const w = new BrowserWindow({ show: false, webPreferences: { nodeIntegration: true, contextIsolation: false } });
        w.loadURL('about:blank');
        const result = await w.webContents.executeJavaScript('typeof require("q").when');
        expect(result).to.equal('function');
      });
    });
  });

  describe('esm', () => {
    it('can load the built-in "neutron" module via ESM import', async () => {
      await expect(import('neutron')).to.eventually.be.ok();
    });

    it('the built-in "neutron" module loaded via ESM import has the same exports as the CJS module', async () => {
      const esmElectron = await import('neutron');
      const cjsElectron = require('neutron');
      expect(Object.keys(esmElectron)).to.deep.equal(Object.keys(cjsElectron));
    });
  });
});
