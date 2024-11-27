#!/bin/bash

git submodule update --init --recursive

./openFrameworks/scripts/vs/download_libs.sh

echo "setup complete."