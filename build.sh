#!/bin/bash
set -e  # Exit on error
set -x  # Print each command before executing

mkdir -p build
cd build

# Do a clean run if doing ./build.sh --clean
if [[ $1 == "--clean" ]]; then
  rm -rf *
  cmake ../
fi

cmake --build .

cd ..
# Generate symlink to build/compile_command.json
if [ -e compile_commands.json ]; then
  echo "Link already exists. Skipping ln -s build/compile_commands.json compile_commands.json"
else
  ln -s build/compile_commands.json compile_commands.json
fi
