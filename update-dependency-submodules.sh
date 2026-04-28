#!/usr/bin/env bash
set -euo pipefail

echo "Updating submodules..."
git submodule update --init --recursive

# CsoundAC should track master.
git -C dependencies/csound-ac fetch --prune origin
git -C dependencies/csound-ac checkout -B master origin/master
git -C dependencies/csound-ac reset --hard origin/master
git -C dependencies/csound-ac log -1 --oneline

# Csound 7 development currently comes from Csound's develop branch.
git -C dependencies/csound fetch --prune origin
git -C dependencies/csound checkout -B develop origin/develop
git -C dependencies/csound reset --hard origin/develop
git -C dependencies/csound log -1 --oneline
