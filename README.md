# numbacruncha

Simple CLI tool that reads a stream of numbers and returns stats on the numbers. The stats are printed in a CSV format for easier handling.

Example usage:

```shell
$ numbacruncha smalltest.txt  | jq
{
  "Sum": 4896345,
  "Count": 1000,
  "Mean": 4896.35,
  "Variance": 1654930883.15,
  "StdDev": 40680.84,
  "Min": 1,
  "Max": 1047427,
  "p25": 4,
  "p30": 5,
  "p40": 7,
  "p50": 14,
  "p60": 34,
  "p70": 234.3,
  "p75": 489.75,
  "p80": 1086.6,
  "p90": 4225.9,
  "p95": 17557.5,
  "p99": 81028.66,
  "p99.9": 546047.88,
  "p99.99": 997289.09
}
```

Currently there are no options or flags that can be used with `numbacruncha`.

## Installation

Run `make` followed by `make install` (may require root). This installs the binary to `/usr/local/bin/`

