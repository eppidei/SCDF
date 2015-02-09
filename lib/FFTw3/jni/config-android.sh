#!/bin/sh

FFTW_VER=$1
shift
FLAGZ=$*

echo "Generating config.h for FFTw3..." >&2

cd ../fftw-$FFTW_VER/
./configure $FLAGZ
cd ../jni

echo "====================================================" >&2
echo "Configured FFTw "$FFTW_VER" \nFlags: "$FLAGZ >&2
echo "====================================================" >&2

