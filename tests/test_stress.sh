#!/bin/bash

if [ ! $1 ]; then
    echo "Usage: test_stress.sh <jarvis>"
else
    $1 --dir1 resources/A/ --dir2 resources/B/ --simpercent 0.3
fi
