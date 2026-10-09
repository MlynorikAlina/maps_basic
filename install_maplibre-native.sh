#Essential libs
sudo apt install libjpeg-dev libpng-dev libcurl4-gnutls-dev libwebp-dev libglfw3-dev libuv1-dev libicu-dev libjpeg-turbo8-dev xvfb libgles-dev

#Build and install
git clone --depth 1 --recurse-submodules -j8 https://github.com/maplibre/maplibre-native.git
cd maplibre-native/
rm -rf build/ CMakeCache.txt
cmake . -B build -DMLN_WITH_OPENGL=ON -DCMAKE_C_COMPILER=gcc-14 -DCMAKE_CXX_COMPILER=g++-14
cmake --build build
sudo cmake --install build/


#Check dependencies
ldd /usr/local/bin/mbgl-render
