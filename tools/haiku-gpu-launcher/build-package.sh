#!/bin/sh
# Builds the summit_gpu package (run on Haiku, where Summit is installed): libsummit_tlsfix.so, the "Summit GPU" launcher with Summit's icon,
# and the hpkg. Usage: ./build-package.sh [output directory]   (default ./out)
set -e
cd "$(dirname "$0")"
OUT=${1:-$PWD/out}
rm -rf "$OUT"
mkdir -p "$OUT/pkg/apps" "$OUT/pkg/lib"
g++ -O2 -fPIC -shared -o "$OUT/pkg/lib/libsummit_tlsfix.so" tlsfix.cpp -lroot
g++ -O2 -o "$OUT/pkg/apps/Summit GPU" launcher.cpp -lroot
# the same icon as Summit (it has the large one only; a vector icon, if a later Summit has one, is copied too)
for ATTR in BEOS:L:STD_ICON BEOS:M:STD_ICON BEOS:ICON; do
    if listattr /boot/system/apps/Summit 2>/dev/null | grep -q "\"$ATTR\""; then
        copyattr -n "$ATTR" /boot/system/apps/Summit "$OUT/pkg/apps/Summit GPU"
    fi
done
cp PackageInfo "$OUT/pkg/.PackageInfo"
package create -C "$OUT/pkg" -i "$OUT/pkg/.PackageInfo" "$OUT/summit_gpu-0.1.0-1-x86_64.hpkg"
echo "built $OUT/summit_gpu-0.1.0-1-x86_64.hpkg"
