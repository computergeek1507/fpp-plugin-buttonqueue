#!/bin/bash

# fpp-plugin-buttonqueue install script

BASEDIR=$(dirname $0)
cd $BASEDIR
cd ..

#sudo apt-get -y update
#sudo apt-get -y install libasio-dev --no-install-recommends

make
