#!/bin/bash
set -e  # Exit on error
set -x  # Print each command before executing

mkdir -p out
cd out

rm -rf *
cmake ../

cmake --build .

# Generate symlink to out/compile_command.json if doing `./build.sh --link`
if [[ $1 == "--link" ]]; then
  ln -s out/compile_commands.json compile_commands.json
fi
