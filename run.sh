#!/bin/bash
set -e  # Exit on error
set -x  # Print each command before executing

mkdir -p out
cd out

# Do a clean run if doing ./run.sh --clean
if [[ $1 == "--clean" ]]; then
  rm -rf *
  cmake ../
fi

cmake --build .
./DuelArenaServer & ./DuelArenaClient

