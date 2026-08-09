#!/bin/bash
set -e

# fpp-plugin-buttonqueue uninstall script

: "${FPPDIR:=/opt/fpp}"
source "${FPPDIR}/scripts/common"
setSetting restartFlag 1

