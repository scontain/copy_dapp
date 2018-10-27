#!/bin/bash
#
# Access to this file is granted under the SCONE SOURCE CODE LICENSE V1.0 
#
# Commercial use of any product using this file requires a commercial 
# license from scontain UG, www.scontain.com.
#
# also see https://sconedocs.github.io
#
# Copyright (C) 2018 Scontain UG
#

set -e

RED='\033[0;31m'
NC='\033[0m'
GREEN='\033[0;32m'

errorexit() {
  echo "${RED}#####  An error occurred while creating simple_demo application image! Please check the logs. #####${NC}"
  exit 1
}

trap 'errorexit' ERR

function verbose {
        printf "${GREEN}$1${NC}\n"
}

verbose "building simple_demo image"

docker build --pull -t sconecuratedimages/iexec:copy_demo .

echo "todo: push image to docker hub"

# not that SCONE_STACK and SCONE_HEAPmust show the same values as define in application.sh - otherwise, MrEnclave will be different!
MRENCLAVE=$(docker run -t --rm --entrypoint "" -e SCONE_ALPINE=1 -e SCONE_STACK=4M -e SCONE_HEAP=40M -e SCONE_HASH=1 sconecuratedimages/iexec:scone_demo /application)

verbose "MRENCLAVE=${MRENCLAVE}"

MRENCLAVE=${MRENCLAVE//[$'\t\r\n ']} envsubst '$MRENCLAVE' < session_template.yml > copy_dapp.yml

# copy to template directory ... image name and short application name

echo "need to sign copy_dapp.yml and push to appstore"

verbose "OK"
