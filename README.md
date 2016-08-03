# 4xClient 
### Gl4thewin
From the root directory run:  
python gl3w_gen.py
### General Pre-reqs
git submodule update --init  
cd src/4xSimulation/tools  
python serialization.py   
cd ../../..
### CMake setup
Then create a build directory and run:  
cmake ..  
make  
### Linux Dependencies (tested on Ubuntu 16.04)
apt-get install libpthread-stubs0-dev  
apt-get install mesa-common-dev  
apt-get install xorg-dev  
