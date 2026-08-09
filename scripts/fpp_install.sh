#!/bin/bash
set -e

# fpp-plugin-buttonqueue install script

BASEDIR=$(dirname $0)
cd $BASEDIR
cd ..

#apt-get -y update
#apt-get -y install libasio-dev --no-install-recommends

make

: "${FPPDIR:=/opt/fpp}"
source "${FPPDIR}/scripts/common"
setSetting restartFlag 1
