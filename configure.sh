#!/bin/bash 

[ -d ./cmake-build ] || mkdir ./cmake-build

cd ./cmake-build

cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_BUILD_TYPE=Debug ..

cp compile_commands.json ..

