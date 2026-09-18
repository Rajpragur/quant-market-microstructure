# Lesson 21 — Order-ID indexing

## Problem

The first order book searched every price level when cancelling an order. That is simple, but it makes cancellation cost grow with the number of price levels and orders inspected.

## Change

`OrderBook` now maintains:

```cpp
std::unordered_map<OrderId, OrderLocation>
```

The map records the order's side and price level. Cancellation can therefore jump directly to the relevant level, then scan only that FIFO queue.

The index is updated when:

- a resting order is inserted;
- an order is cancelled;
- a resting order is completely filled.

Duplicate live order IDs and zero-quantity orders are rejected.

## Complexity

| Operation | Before | Now |
|---|---:|---:|
| Find best bid/ask | O(1) | O(1) |
| Find price level | O(log P) | O(log P) |
| Find order's price level for cancel | O(P) scan | O(1) average hash lookup |
| Find order within its FIFO level | O(Q) | O(Q) |
| Insert into FIFO level | O(1) | O(1) average plus index update |

`P` is the number of price levels and `Q` is the number of orders at the selected level.

## Why this is not automatically faster

The index adds a hash-table lookup and memory access to every accepted order. The benchmark moved from roughly 15M to roughly 13M synthetic submissions/sec after this change. That is an expected trade-off: cancellation becomes more targeted, while insertion does more work.

The next step is to benchmark realistic mixes of adds, cancels, modifies, and matches. We should optimize only after measuring the workload that matters.
