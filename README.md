# Jarvis

## Building

Clone this repo recursive to fetch fmt lib!

Standart cmake build

## Requirements:
* C++20
* OpenMP
* Python (for tests only)
* Linux

## Similarity algorithm

[Basic idea](https://www.youtube.com/watch?v=WiCRonv7tcM&list=PL4_hYwCyhAvaAajkilVk9Pm_xDGnwLR-3&index=8) + mulithreading via OpenMP

Complexity: $O(n\log n)$, where n in max file size

FFT implementation was taken from my previous semesters

## Testing

* `cd tests`
* `./gen_data.sh`
* `./test_simple.sh <your_jarvis>`
* `./test_stress.sh <your_jarvis>`


