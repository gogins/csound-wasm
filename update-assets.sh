#!/bin/bash 
echo "Bringing in assets from manual and cmask..."
rm -rf csound-assets
mkdir -p csound-assets
cp /usr/local/share/samples/* csound-assets/
find ~/csound-extended-wasm/dependencies/cmask/examples -name "*.aif"  -exec cp {} csound-assets/ \;
find ~/csound-extended-manual/manual/examples -name "*.aiff" -exec cp {} csound-assets/ \;
find ~/csound-extended-manual/manual/examples -name "*.wav" -exec cp {} csound-assets/ \;
find ~/csound-extended-manual/manual/examples -name "*.sf2" -exec cp {} csound-assets/ \;
find ~/csound-extended-manual/manual/examples -name "128*" -exec cp {} csound-assets/ \;
find ~/csound-extended-manual/manual/examples -name "*.ats" -exec cp {} csound-assets/ \;
find ~/csound-extended-manual/manual/examples -name "*.lpc" -exec cp {} csound-assets/ \;
find ~/csound-extended-manual/manual/examples -name "*.mid" -exec cp {} csound-assets/ \;
find ~/csound-extended-manual/manual/examples -name "*.matrix" -exec cp {} csound-assets/ \;
ls -ll csound-assets
echo "Finished bringing in assets from manual and cmask."
