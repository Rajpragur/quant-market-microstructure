# Quant Market Microstructure

A small, from-scratch C++20 market-microstructure lab focused on the part of trading systems that is easy to get wrong: **what actually happens between a market event, an order, and a fill**.

The project started as a way to learn low-latency C++ properly. It now contains a price-time-priority limit-order book, deterministic event replay, queue-aware execution simulation, and a read-only Kalshi data adapter.

The main question behind the project is simple:

> If a signal looks attractive on paper, does it still survive queue position, latency, partial fills, fees, and adverse selection?

This is a research and simulation project. It does not place live orders or claim live profitability.

## What is here

### C++20 core

- Price-time-priority limit-order book
- FIFO order queues at each price level
- Limit-order matching and partial fills
- Best bid/ask and level depth queries
- Duplicate order-ID validation
- Indexed cancellation
- Order modification with queue-priority rules
- Fixed-point integer prices instead of floating-point prices
- SPSC queue using acquire/release atomics

### Replay and execution research

- Versioned binary market-event format
- Explicit field-by-field serialization
- Replay magic/version checks
- Truncation and unreasonable-file detection
- Replay command-line tool
- Queue-position simulation
- Exchange latency and order availability time
- Partial fills
- Integer-tick fees
- Inventory and cash tracking
- Spread, mid-price, microprice, and imbalance features

### Kalshi integration

`tools/kalshi_snapshot.py` reads public Kalshi market metadata and order-book snapshots. It is intentionally read-only and does not require API credentials for the public REST endpoints.

The adapter is kept separate from execution code. Nothing in this repository can place a live order.

## Design choices

### Why C++20?

The hot path is deliberately written in C++ so that data layout, ownership, allocations, atomics, and latency are visible rather than hidden behind a framework.

### Why fixed-point prices?

Prices are represented as integer ticks. This avoids floating-point equality problems and makes matching, serialization, fees, and replay deterministic.

### Why an SPSC queue?

The simplest fast pipeline has one producer and one consumer:

```text
market-data reader → SPSC queue → order-book owner
```

The ownership model avoids having several threads mutate the same book and keeps synchronization limited to the queue boundary.

### Why start with `std::map` and `std::deque`?

They make price ordering and FIFO behaviour easy to inspect and test. The project measures before replacing them with more complicated structures. A faster data structure is only useful if it improves the workload that matters.

## Build and run

Requirements:

- C++20 compiler
- CMake 3.20+
- Python 3.10+

From the repository root:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
python3 tests/test_kalshi_snapshot.py
```

Or run the complete local check:

```bash
./verify.sh
```

Run the synthetic order-book benchmark:

```bash
./build/bench_order_book
```

Run the replay tool:

```bash
./build/replay_events path/to/events.bin
```

Generate the read-only research report and start the local API:

```bash
python3 tools/generate_report.py
python3 tools/demo_server.py --port 8080
```

Open `http://127.0.0.1:8080`. The API exposes only:

```text
GET /api/health
GET /api/report
```

There are no trading controls or credential inputs.

## Benchmark note

The synthetic benchmark is useful for comparing changes on the same machine. It is not an exchange-performance claim. Results depend on the compiler, build type, CPU, workload, and data distribution, so every number should be reported with that context.

## Project status

The repository is deliberately small and inspectable. The current implementation is suitable for studying and demonstrating:

- low-level C++ design
- market-data state reconstruction
- order-book invariants
- deterministic simulation
- execution modelling
- quantitative research hygiene

The important remaining research work is not adding decorative strategy code. It is validating the simulator against real historical event streams and showing which apparent opportunities disappear once execution costs are modelled.

## License

No license has been selected yet. Until one is added, the repository is available for viewing and study, but reuse should be treated as not automatically permitted.
