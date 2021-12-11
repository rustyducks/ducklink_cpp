# ducklink_cpp

A C++ library to communicate in Ducklink over UDP or Serial.

## Installation

### Install dependencies

- **[Protoduck](https://github.com/rustyducks/protoduck)**
- **serialib**
- **Protobuf** 3.0.0

Both Protoduck and serialib are included as submodules of this repository, and compiled with the main project:

```bash
git submodule update --init --recursive
```

Protobuf can be installed via the package manager, but for crosscompiling, it needs to be compiled from the source.

Binaries:

```bash
sudo apt update && sudo apt install protobuf-compiler
```

### Build and install

```bash
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=</path/to/your/workspace> ..
make -j4
make install
```

### Crosscompiling for Linux ARM

Protobuf needs to be compiled for Linux ARM and installed in the specified workspace.

```bash
mkdir buildarm && cd buildarm
export LINUX_ARM_TOOLCHAIN_PATH=</path/to/toolchain>  # optional: defaults to /usr/lib/ccache
cmake -DCROSSCOMPILE_ARM=ON -DCMAKE_INSTALL_PREFIX=</path/to/your/workspace/armlinux> ..  # your workspace must contain the dependencies, manually built for the host
make -j4
make install
```