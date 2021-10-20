#!/bin/bash

#start notifications
echo debug.main | entr notify-send "Compiler finished" &

#start compiler for submodule
cd ./modules/RGSDL
./compile_server.sh &
cd ../..

#listen to changes to source or librarys
while sleep 0.1 ; do
    ls src/* ./modules/*/lib/* | entr -d make
done
