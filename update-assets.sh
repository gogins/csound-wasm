#!/bin/bash 
echo "Bringing in assets from manual and cmask..."
rm -rf csound-assets
mkdir -p csound-assets
cp /usr/local/share/samples/* csound-assets/
find dependencies/csound-ac/dependencies/cmask/examples -name "*.aif"  -exec cp -v {} csound-assets/ \;
find ~/csound-extended-manual/manual/examples -name "*.aiff" -exec cp -v {} csound-assets/ \;
find ~/csound-extended-manual/manual/examples -name "*.wav" -exec cp -v {} csound-assets/ \;
find ~/csound-extended-manual/manual/examples -name "*.sf2" -exec cp -v {} csound-assets/ \;
find ~/csound-extended-manual/manual/examples -name "128*" -exec cp -v {} csound-assets/ \;
find ~/csound-extended-manual/manual/examples -name "*.ats" -exec cp -v {} csound-assets/ \;
find ~/csound-extended-manual/manual/examples -name "*.lpc" -exec cp -v {} csound-assets/ \;
find ~/csound-extended-manual/manual/examples -name "*.mid" -exec cp -v {} csound-assets/ \;
find ~/csound-extended-manual/manual/examples -name "*.matrix" -exec cp -v {} csound-assets/ \;
ls -ll csound-assets
echo "Finished bringing in assets from manual and cmask."
