#!/bin/sh
# Builds libsummit_tlsfix.so and installs it with the summit-gl launcher into ~/config/non-packaged (run on Haiku).
set -e
cd "$(dirname "$0")"
LIBDIR=/boot/home/config/non-packaged/lib
BINDIR=/boot/home/config/non-packaged/bin
mkdir -p "$LIBDIR" "$BINDIR"
g++ -O2 -fPIC -shared -o "$LIBDIR/libsummit_tlsfix.so" tlsfix.cpp -lroot
cp -f summit-gl "$BINDIR/summit-gl"
chmod 755 "$BINDIR/summit-gl"
echo "installed $LIBDIR/libsummit_tlsfix.so and $BINDIR/summit-gl"
echo "start Summit with:  summit-gl  (or use the 'Summit (GPU)' entry in Deskbar's Applications menu, see README.txt)"
