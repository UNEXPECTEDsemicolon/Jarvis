#!/bin/bash

if [ ! $1 ]; then
    echo "Usage: test_simple.sh <jarvis>"
else
    $1 --dir1 resources/C/ --dir2 resources/D/ --simpercent 0.3
    $1 --dir1 resources/E/ --dir2 resources/F/ --simpercent 0.3
fi
