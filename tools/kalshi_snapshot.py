#!/usr/bin/env python3
"""Fetch public Kalshi market metadata or an order-book snapshot."""
from __future__ import annotations

import argparse
import json
import sys
from urllib.error import HTTPError, URLError
from urllib.parse import urlencode
from urllib.request import Request, urlopen

BASE_URL = "https://external-api.kalshi.com/trade-api/v2"


def get_json(path: str, params: dict[str, str] | None = None) -> dict:
    query = f"?{urlencode(params)}" if params else ""
    request = Request(
        f"{BASE_URL}{path}{query}",
        headers={"Accept": "application/json", "User-Agent": "mdp-kalshi-adapter/0.1"},
    )
    with urlopen(request, timeout=20) as response:
        return json.load(response)


def snapshot(ticker: str) -> dict:
    payload = get_json(f"/markets/{ticker}/orderbook")
    book = payload.get("orderbook_fp", {})
    return {
        "venue": "kalshi",
        "ticker": ticker,
        "yes_bids": book.get("yes_dollars", []),
        "no_bids": book.get("no_dollars", []),
    }


def markets(limit: int) -> dict:
    payload = get_json("/markets", {"limit": str(limit), "status": "open"})
    return {
        "venue": "kalshi",
        "markets": [
            {
                "ticker": item.get("ticker"),
                "title": item.get("title"),
                "status": item.get("status"),
                "yes_bid": item.get("yes_bid_dollars"),
                "yes_ask": item.get("yes_ask_dollars"),
                "last_price": item.get("last_price_dollars"),
            }
            for item in payload.get("markets", [])
        ],
    }


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--ticker", help="fetch one market's order-book snapshot")
    parser.add_argument("--limit", type=int, default=10, help="number of open markets to list")
    args = parser.parse_args()
    try:
        result = snapshot(args.ticker) if args.ticker else markets(args.limit)
    except (HTTPError, URLError, TimeoutError, json.JSONDecodeError) as error:
        print(f"kalshi request failed: {error}", file=sys.stderr)
        return 1
    print(json.dumps(result, indent=2, sort_keys=True))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
