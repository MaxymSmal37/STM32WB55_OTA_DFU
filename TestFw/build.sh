#!/usr/bin/env bash
set -euo pipefail

project_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
build_dir="${BUILD_DIR:-"$project_dir/build"}"
build_type="${BUILD_TYPE:-Debug}"

cmake -S "$project_dir" -B "$build_dir" -DCMAKE_BUILD_TYPE="$build_type"
cmake --build "$build_dir" --parallel
