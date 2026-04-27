#!/bin/bash 
echo "Updating all submodules for csound-wasm..."

git submodule update --init --recursive

(
    cd dependencies/csound-ac

    # Make sure origin exists and fetch the branch tip
    git remote -v
    git fetch --prune origin +refs/heads/master:refs/remotes/origin/master

    # Create/update local branch to point at the remote branch tip
    git checkout -B main origin/master

    # Hard reset to ensure you are exactly at the latest remote commit
    git reset --hard origin/master

    git status
    git log -1 --oneline
)

(
    cd dependencies/csound
    git fetch --prune origin
    git checkout -B develop origin/develop
    git reset --hard origin/develop
    git log -1 --oneline
)

pwd
echo "Finished updating all submodules for csound-wasm."