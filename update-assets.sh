#!/usr/bin/env bash
set -euo pipefail

echo "Bringing in optional Csound assets..."
rm -rf csound-assets
mkdir -p csound-assets

copy_matches()
{
    local root="$1"
    local pattern="$2"

    if [[ -d "${root}" ]]; then
        find "${root}" -name "${pattern}" -exec cp -v {} csound-assets/ \;
    fi
}

if [[ -d /usr/local/share/samples ]]; then
    cp /usr/local/share/samples/* csound-assets/ || true
fi

copy_matches dependencies/csound-ac/dependencies/cmask/examples "*.aif"
copy_matches "$HOME/csound-extended-manual/manual/examples" "*.aiff"
copy_matches "$HOME/csound-extended-manual/manual/examples" "*.wav"
copy_matches "$HOME/csound-extended-manual/manual/examples" "*.sf2"
copy_matches "$HOME/csound-extended-manual/manual/examples" "128*"
copy_matches "$HOME/csound-extended-manual/manual/examples" "*.ats"
copy_matches "$HOME/csound-extended-manual/manual/examples" "*.lpc"
copy_matches "$HOME/csound-extended-manual/manual/examples" "*.mid"
copy_matches "$HOME/csound-extended-manual/manual/examples" "*.matrix"

ls -ll csound-assets
