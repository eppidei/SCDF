#!/bin/sh
javah -classpath ../../project/TestAndroid/bin/classes/ -d . it.scdf.framework.Scdf

# build the app at least once when changing Scdf class stuff
# before launching this script!