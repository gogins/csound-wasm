#!/bin/bash 
echo "Updating all submodules for csound-wasm..."
git submodule update --init --recursive --remote
git submodule update --recursive
git submodule status --recursive
cd dependencies/csound-ac
git checkout master
git branch
cd ../..
pwd
echo "Finished updating all submodules for csound-wasm."
