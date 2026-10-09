#Основные зависимости для сборки (Ubuntu/Debian)
#sudo apt install build-essential

#Making vector tiles
sudo apt install tilemaker

cp /usr/share/doc/tilemaker/examples/process-coastline.lua ./data/process-coastline.lua
cp /usr/share/doc/tilemaker/examples/process-openmaptiles.lua ./data/process-openmaptiles.lua

#In case one that was provided causes errors
#cp /usr/share/doc/tilemaker/examples/config-openmaptiles.json ./data/config-openmaptiles.json

