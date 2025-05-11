# Duel Arena
Final Thesis project: a networked client-server game using SDL2, SDL_net, ImGui, and MessagePack for serialization.

## Prerequisites
Before building and running **Duel Arena**, ensure you have the following tools and libraries installed on your system:

- **C++17 compiler** (e.g., `g++-13` or later)
- **CMake** ≥ 3.22.1
- **GNU Make**
- **SDL2** development libraries
- **SDL_net** development libraries
- **msgpack-cxx** (MessagePack C++ library)
- **OpenGL** development headers
- **Git** (for cloning and submodules)

### Installing via apt (Debian/Ubuntu)
```bash
sudo apt update
sudo apt install -y \
    build-essential \
    cmake \
    libsdl2-dev \
    libsdl2-net-dev \
    libmsgpack-dev \
    libgl1-mesa-dev
```

> **Note**: `libmsgpack-dev` provides the C library only. To get the C++ bindings, you may still need to build from source (see below). Similar situation with `libsdl2-net-dev`. If you don't install these from source, you may need to add additional CMAKE flags, meaning `./build.sh` and `./run.sh` won't work as intended!

### Installing `libsdl2-net-dev` and `libmsgpack-dev` and from Source
If you prefer or require the latest versions:

1. **SDL_net**
    ```bash
    cd ~
    git clone https://github.com/libsdl-org/SDL_net.git
    cd SDL_net
    git checkout SDL2
    mkdir build && cd build
    cmake .. -DCMAKE_INSTALL_PREFIX=$HOME/.local
    make -j$(nproc)
    make install
    export PKG_CONFIG_PATH="$HOME/.local/lib/pkgconfig:$PKG_CONFIG_PATH"
    ```

2. **MessagePack C++ (msgpack-cxx)**
    ```bash
    cd ~
    git clone https://github.com/msgpack/msgpack-c.git
    cd msgpack-c
    git checkout cpp_master
    mkdir build && cd build
    cmake .. -DCMAKE_INSTALL_PREFIX=$HOME/.local -DMSGPACK_CXX=ON
    make -j$(nproc)
    make install
    export PKG_CONFIG_PATH="$HOME/.local/lib/pkgconfig:$PKG_CONFIG_PATH"
    ```

3. **ImGui** (already included as a submodule)
    ```bash
    git submodule update --init --recursive
    ```

> Make sure `$HOME/.local/lib/pkgconfig` is in your `PKG_CONFIG_PATH` so that `find_package` can locate SDL_net and msgpack-cxx.

## Cloning the Repository
```bash
git clone https://github.com/savipauk/duel-arena.git
cd duel-arena
git submodule update --init --recursive
```


## Build & Run
Two helper scripts are provided for convenience:

- **build.sh**: Generate build files
  ```bash
  ./build.sh [--clean]
  ```
  - `--clean`: removes previous build directory before regenerating
  - Output: `build/` directory and a `compile_commands.json`

- **run.sh**: Build and run the project
  ```bash
  ./run.sh [--clean]
  ```
  - Combines the steps of `build.sh` and launching two client instances and server

### Manual Build (CMake + Make)
Alternatively, you can build manually:

```bash
mkdir -p build && cd build
cmake ..                          # or: cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

After building, two executables will be available in `build/`:
- `DuelArenaClient`
- `DuelArenaServer`

### Running the Game
Open two terminal windows (or tabs):

1. **Server**
    ```bash
    cd build
    ./DuelArenaServer
    ```
    - Runs at port 50325

2. **Client**
    ```bash
    cd build
    ./DuelArenaClient
    ```

Example:
```bash
# Terminal 1
./DuelArenaServer

# Terminal 2
./DuelArenaClient 

# Terminal 3
./DuelArenaClient 
```

## Project Structure
```
├── build.sh            # Helper: generate build files
├── run.sh              # Helper: build and run
├── CMakeLists.txt      # Main CMake configuration
├── client/             # Client-side source & headers
├── server/             # Server-side source & headers
├── common/             # Shared code (protocols, utilities)
├── third_party/        # External libraries (ImGui, etc.)
└── build/              # Build files and generated compile_commands.json
```

## License
This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.
