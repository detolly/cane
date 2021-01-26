# cane
## Project Description

This project is a work in progress level editor for the Sly Cooper games.

Cane supports building on Windows and Linux.

Prerequisites:
 * cmake
 * git

## Getting Started - Linux (Ubuntu/Debian)

Install Packages and Init Repository
```bash
sudo apt-get install cmake git build-essential
```

Compile
```bash
mkdir build && cd build && cmake .. && cmake --build --config Release
```

Run
```bash
./cane
```

## Getting Started - Linux (Arch)
Install Packages and Init Repository
```bash
pacman -S cmake git base-devel
```

Compile
```bash
mkdir build && cd build && cmake .. && cmake --build . --config Release
```

Run
```bash
./cane
```
