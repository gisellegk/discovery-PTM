https://www.learnopencv.com/install-opencv3-on-windows/


# create build directory
mkdir build
cd build
# create Visual Studio project files using cmake
cmake -G "Visual Studio 15 2017 Win64" ..

# build our application
cmake --build .\build --config Release
# once the build is complete, it will generate exe file in build\Release directory

.\build\Release\lpmaker.exe .\sampleimg\fish_fossil_01.jpg
