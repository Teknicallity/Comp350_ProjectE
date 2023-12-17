#!/bin/bash

if [ $# -ne 1 ]; then
    echo "Must have exactly one argument: project letter"
    exit 2
fi

tar -cvf "Sheputa_Comp350_Project$1.tar" *

