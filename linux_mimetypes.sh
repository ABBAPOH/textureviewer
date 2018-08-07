#!/bin/bash

USER=`whoami`
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"
MIME_DIR=/usr/share/mime/

if [ $USER != "root" ]; then
    echo "Must be called as root"
    exit 1
fi

cp $DIR/src/libs/extramimetypes/x-*.xml $MIME_DIR/packages
update-mime-database $MIME_DIR
