# Quant Market Microstructure Lab

A from-scratch C++20 project for learning low latency systems, concurrency, and market data processing.

This project is being implemented independently so every design choice can be understood and defended.

## Current milestone

A bounded single producer single consumer queue with:

- FIFO behavior
- Fixed capacity
- Explicit full and empty behavior
- Acquire and release atomics
- CTest coverage

The next layer is a deterministic limit-order-book and replay engine. The goal
is not to build another toy strategy or claim unrealistic backtest returns. It
is to measure what market-microstructure signals survive realistic queue
position, latency, partial fills, fees, and adverse selection.

The first order-book slice is now working in `include/mdp/order_book.hpp` and
is covered by `tests/test_order_book.cpp`. It implements price-time priority,
partial fills, cancellation, and best bid/ask queries.

Lesson 21 adds an order-ID location index for duplicate-ID validation and
targeted cancellation. The trade-off is documented in
`docs/lesson21_order_index.md` and measured rather than assumed to be faster.

Kalshi is the first external venue adapter. `tools/kalshi_snapshot.py` reads
public market metadata or a single order-book snapshot without credentials:

```bash
python3 tools/kalshi_snapshot.py --limit 10
python3 tools/kalshi_snapshot.py --ticker KXEXAMPLE
```

The adapter is deliberately read-only. Kalshi's real-time WebSocket stream
requires signed API-key headers, so credentials will not be needed for the
public demo and will never be committed to this repository.

## Build and test

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
```

Run the first benchmark with:

```bash
./build/bench_order_book
```

Run the complete local verification:

```bash
./verify.sh
```

Generate the read-only research report and start the local demo:

```bash
python3 tools/generate_report.py
python3 tools/demo_server.py --port 8080
```

Then open `http://127.0.0.1:8080`. The API endpoints are `/api/health` and
`/api/report`. The server contains no trading controls and requires no
credentials.

The production-style React frontend lives in `frontend/` and does not use
Next.js:

```bash
cd frontend
npm install
npm run dev
```

For a static deployment:

```bash
npm run build
```

Deploy `frontend/dist/` to any static host. The UI is read-only and falls back
to embedded fixture data when the API is not available.

Replay a versioned binary event file:

```bash
./build/replay_events path/to/events.bin
```

The benchmark reports order submissions per second and trade count. Results
must always be reported together with the machine, compiler, build type, and
workload; a single throughput number is not a claim about exchange-level
performance.

## Learning path

1. Finish the deterministic order-book API and matching edge cases
2. Add binary event capture and deterministic replay
3. Normalize Kalshi prediction contracts into the same event/replay schema
4. Add order-flow imbalance, microprice, and queue-position features
5. Add latency-, fee-, and partial-fill-aware cross-venue execution simulation
6. Add walk-forward evaluation with leakage checks
7. Publish reproducible reports and a read-only demo API
8. Add Linux CI and cross-machine benchmark comparisons

The repository includes `.github/workflows/ci.yml`, which builds all C++
targets, runs CTest, runs the Kalshi fixture test, and generates the report on
every push and pull request.
