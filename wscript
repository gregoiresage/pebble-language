#
# This file is the default set of rules to compile a Pebble project.
#
# Feel free to customize this to your needs.
#

import os.path
from enamel.enamel import enamel
from gen_dict import gen_loc_dict
from dict2bin import dict_2_bin
import json

top = '.'
out = 'build'


def options(ctx):
    ctx.load('pebble_sdk')


def configure(ctx):
    """
    This method is used to configure your build.
    ctx.load(`pebble_sdk`) automatically configures a build for each valid platform in `targetPlatforms`.
    Platform-specific configuration: add your change after calling ctx.load('pebble_sdk') and make sure to set the
    correct environment first.
    Universal configuration: add your change prior to calling ctx.load('pebble_sdk').
    """
    ctx.load('pebble_sdk')

    # generate locale_default.json
    json_dict = gen_loc_dict('src') 
    json.dump(json_dict, open('resources/locale_default.json', "wb"), indent=2, sort_keys=True)

    # generate locale_XXX.bin files
    for subdir, dirs, files in os.walk('resources'):
        for file in files:
            if file.endswith(".json") and file.startswith("locale_") : 
                dict_2_bin(subdir + os.sep + file)


def build(ctx):
    ctx.load('pebble_sdk')

    build_worker = os.path.exists('worker_src')
    binaries = []

    for p in ctx.env.TARGET_PLATFORMS:
        ctx.set_env(ctx.all_envs[p])
        ctx.set_group(ctx.env.PLATFORM_NAME)
        app_elf = '{}/pebble-app.elf'.format(ctx.env.BUILD_DIR)
        ctx(rule = enamel, source='src/js/config.json', target=['enamel.c', 'enamel.h'])
        ctx.pbl_program(source=ctx.path.ant_glob('src/**/*.c') + ['enamel.c'], target=app_elf)

        if build_worker:
            worker_elf = '{}/pebble-worker.elf'.format(ctx.env.BUILD_DIR)
            binaries.append({'platform': p, 'app_elf': app_elf, 'worker_elf': worker_elf})
            ctx.pbl_worker(source=ctx.path.ant_glob('worker_src/**/*.c'), target=worker_elf)
        else:
            binaries.append({'platform': p, 'app_elf': app_elf})

    ctx.set_group('bundle')
    ctx.pbl_bundle(binaries=binaries, js=ctx.path.ant_glob(['src/js/**/*.js', 'src/js/**/*.json']), js_entry_file='src/js/app.js')
