import importlib.util
from pathlib import Path

spec = importlib.util.spec_from_file_location(
    "kalshi_snapshot", Path(__file__).parents[1] / "tools" / "kalshi_snapshot.py"
)
module = importlib.util.module_from_spec(spec)
assert spec.loader is not None
spec.loader.exec_module(module)

responses = {
    "/markets": {"markets": [{
        "ticker": "KXTEST",
        "title": "Will the test pass?",
        "status": "open",
        "yes_bid_dollars": "0.41",
        "yes_ask_dollars": "0.44",
        "last_price_dollars": "0.43",
    }]},
    "/markets/KXTEST/orderbook": {"orderbook_fp": {
        "yes_dollars": [["0.41", "12"]],
        "no_dollars": [["0.56", "8"]],
    }},
}

module.get_json = lambda path, params=None: responses[path]
assert module.markets(10) == {
    "venue": "kalshi",
    "markets": [{
        "ticker": "KXTEST",
        "title": "Will the test pass?",
        "status": "open",
        "yes_bid": "0.41",
        "yes_ask": "0.44",
        "last_price": "0.43",
    }],
}
assert module.snapshot("KXTEST")["yes_bids"] == [["0.41", "12"]]
assert module.snapshot("KXTEST")["no_bids"] == [["0.56", "8"]]
print("Kalshi adapter fixture tests passed")
