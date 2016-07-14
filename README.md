# 4xClient 

## Linux Pre-requirements (Ubuntu 16.04)
apt-get install libpthread-stubs0-dev
apt-get install mesa-common-dev
apt-get install xorg-dev

## General Pre-reqs
git submodule update --init
cd src/4xSimulation/tools
python type_generator.py
cd ../../..

## Gl4thewin
From the root directory run:

python gl3w_gen.py

## CMake setup
Then to build the project files run:

mkdir build

cd build

cmake ..\

make
