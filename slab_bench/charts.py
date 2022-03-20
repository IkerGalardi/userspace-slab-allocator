#!/bin/python3

import csv
import sys

if len(sys.argv) != 2:
    print("Wrong command line arguments")
    print("  Usage: ./charts.py <benchmark.csv>")
    exit(1)

print(sys.argv[1])
