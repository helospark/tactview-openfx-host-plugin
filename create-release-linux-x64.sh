#!/bin/bash

set -e

cd native
echo "Compiling native code..."
./build.sh
cd ..
mvn clean install
