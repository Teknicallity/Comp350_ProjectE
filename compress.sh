#!/bin/bash

if [[ $# -ne 1 ]]; then
    echo "Must have exactly one argument: name of tar file"
    exit 2
fi

tar -cvf $1 *

