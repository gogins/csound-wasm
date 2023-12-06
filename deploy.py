#!python3
'''
Deploying CsoundAudioNode, CsoundAC, and some extras to dependent projects...
'''
print(__doc__)

import shutil
import os

dist = os.path.join("csound-wasm", "dist-wasm")
home = os.path.expanduser("~")
source_directory = os.path.join(home, dist )
os.chdir(source_directory)
print('Changed directory to "{}"\n'.format(os.getcwd()))

targets = []
targets.append(os.path.join(home, "cloud-5/public"))
targets.append(os.path.join(home, "csound-ac/silencio/js"))
targets.append(os.path.join(home, "csound-android/CsoundForAndroid/CsoundForAndroid/CsoundApplication/src/main/assets/examples/Gogins"))
targets.append(os.path.join(home, "csound-examples/docs"))
targets.append(os.path.join(home, "gogins.github.io"))
targets.append(os.path.join(home, "nwbuild-cloud-5/src"))

deployables = '''CsoundAC.js
CsoundAudioNode.js
CsoundAudioProcessor.js
csound_loader.js
minimal.html
patches
trichord_space.html'''

targets_ = '''
~/cloud-5/public/
~/gogins.github.io/'''

for target in targets:
    for deployable in deployables.splitlines():
        print ('copying {} to {}'.format(deployable, target))
        if os.path.isfile(deployable):
            shutil.copy(deployable, target)
        if os.path.isdir(deployable):
            target_dir = os.path.join(target, deployable)
            shutil.copytree(deployable, target_dir, dirs_exist_ok=True)
    files = os.listdir(target)
    for file in files:
        print('    {}: {}'.format(target, file))
