#!/bin/bash 
echo "Updating all submodules for csound-wasm..."

git submodule update --init --recursive

(
    cd dependencies/csound-ac

    # Make sure origin exists and fetch the branch tip
    git remote -v
    git fetch --prune origin +refs/heads/chord-space-gluing:refs/remotes/origin/chord-space-gluing

    # Create/update local branch to point at the remote branch tip
    git checkout -B chord-space-gluing origin/chord-space-gluing

    # Hard reset to ensure you are exactly at the latest remote commit
    git reset --hard origin/chord-space-gluing

    git status
    git log -1 --oneline
)

(
    cd dependencies/csound
    git fetch --prune origin
    git checkout -B csound6 origin/csound6
    git reset --hard origin/csound6
    git log -1 --oneline
)

pwd
echo "Finished updating all submodules for csound-wasm."