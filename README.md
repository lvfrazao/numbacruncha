# numbacruncha

Simple CLI tool that reads a stream of numbers and returns stats on the numbers. The stats are printed in a CSV format for easier handling.

Example usage:

```shell
$ numbacruncha smalltest.txt
Sum,Count,Mean,Variance,StdDev,Min,Max,p25,p30,p40,p50,p60,p70,p75,p80,p90,p95,p99,p99.9,p99.99
4896345.00,1000,4896.35,1654930883.15,40680.84,1.00,1047427.00,4.00,5.00,7.00,14.00,34.00,234.30,489.75,1086.60,4225.90,17557.50,81028.66,546047.88,997289.09 
```

Currently there are no options or flags that can be used with `numbacruncha`.

## Installation

Run `make` followed by `make install` (may require root). This installs the binary to `/usr/local/bin/`

