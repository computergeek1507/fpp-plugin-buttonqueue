#!/bin/sh

echo "Running fpp-plugin-buttonqueue PreStart Script"

BASEDIR=$(dirname $0)
cd $BASEDIR
cd ..
make
