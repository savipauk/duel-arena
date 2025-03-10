#!/bin/bash
set -e  # Exit on error
set -x  # Print each command before executing

mkdir -p out
cd out

# Do a clean run if doing ./build.sh --clean
if [[ $1 == "--clean" ]]; then
  rm -rf *
  cmake ../
fi

cmake --build .

# Generate symlink to out/compile_command.json if doing `./build.sh --link`
if [ -e compile_commands.json ]; then
  echo "Link already exists. Skipping ln -s out/compile_commands.json compile_commands.json"
else
  ln -s out/compile_commands.json compile_commands.json
fi
