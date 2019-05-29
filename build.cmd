@echo off
if not exist build\ md build
cd build
rm -rf *
cmake -G"Visual Studio 15 2017 Win64" ..
cmake --build . --config Release
cd ..