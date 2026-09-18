#!/usr/bin/env bash
set -euo pipefail
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
python3 tests/test_kalshi_snapshot.py
python3 tools/generate_report.py
printf 'verification passed\n'
