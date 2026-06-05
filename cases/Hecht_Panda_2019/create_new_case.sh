#!/bin/bash
# Create a new cryogenic_H2_test case by copying 0.orig/ constant/ system/ from a
# source case.
#
# Usage:
#   ./create_new_case.sh <old_number> <new_number>
#
# Examples:
#   ./create_new_case.sh 01 02
#   ./create_new_case.sh 01 03

set -euo pipefail

if [ $# -ne 2 ]; then
    echo "Usage: $0 <old_number> <new_number>"
    exit 1
fi

OLD=$1
NEW=$2

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
SRC="$SCRIPT_DIR/cryogenic_H2_test_${OLD}"
DST="$SCRIPT_DIR/cryogenic_H2_test_${NEW}"

if [ ! -d "$SRC" ]; then
    echo "Error: source case not found: $SRC"
    exit 1
fi
if [ -d "$DST" ]; then
    echo "Error: target already exists: $DST"
    exit 1
fi

echo "Source : $SRC"
echo "Target : $DST"
mkdir -p "$DST"

for folder in 0.orig constant system; do
    if [ -d "$SRC/$folder" ]; then
        cp -r "$SRC/$folder" "$DST/"
        echo "  copied $folder/"
    else
        echo "  Warning: $folder/ not found in source — skipped"
    fi
done

for script in Allrun Allclean; do
    if [ -f "$SRC/$script" ]; then
        cp "$SRC/$script" "$DST/"
        echo "  copied $script"
    fi
done

echo ""
echo "Done — $DST is ready."
echo "Next steps:"
echo "  cd $DST"
echo "  bash Allrun"
