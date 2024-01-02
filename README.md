## Leaf

Leaf is physics engine for 2D games.

#### Dependenices

```sh
# Intall OpenGL libraries
sudo apt install mesa-utils libglu1-mesa-dev freeglut3-dev mesa-common-dev
sudo apt install libao-dev libmpg123-dev

```

#### Submodules
```sh
# Leaf depends on glfw and glm (for now)
# After cloning repository install both dependenies with command
git submodule update --init --recursive


```
#### Build Using CMAKE
```sh
mkdir build
cd build
cmake ../
# On Windows use nmake
nmake .
# On Linux use GNU make
make .

# Go back to project directory and run app
cd ..
./build/leaf
```
