Download CLAPACK from netlib:
http://www.netlib.org/clapack/clapack.tgz
modify the version in the android makefiles if you got a different one

CURRENT lapack version: 3.2.1

BUILD BLAS:
launch ndk-build in jni dir, or include the makefile in the Android.mk of the app needing the lib.
