#!/bin/bash

set -e

cd native
echo "Compiling native code..."
cmd /c build.bat
cd ..

mvn clean install
