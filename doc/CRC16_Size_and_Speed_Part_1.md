# CRC16 Size and Speed Part 1
CRC's are used in all kind of applications like verification, integrity checks, etc. Although `CRC8` and `CRC32` are also commonly used, we use `CRC16` to illustrate the speed and size optimalisation/penalty when using it.

For PCs, Desktop and Laptops a slower calculation is not a huge problem, unless lots of data need to be processed. Memory is not a big issue when using tables and otherwise the computational speed is high enough to keep up when used in communication.

For embedded controllers it is different as memory is usually much more constrained and clock speeds much lower than desktop machines. We go from Gigabytes to kilobytes and from Gigahertz to Megahertz...

_Part-2 will address the use of CRC on STM32 microcontrollers._

**So how fast is it and what size will it be?**

## CRC16 comparison
Using two implementations of CRC16 calculations we tested it on a MacBook Pro M3 MAX 64GB. You can test it yourself. The program should compile under any vanilla C-compiler. See the following text and results:

```bash
$ cc --version
Apple clang version 21.0.0 (clang-2100.1.1.101)
Target: arm64-apple-darwin25.6.0
Thread model: posix
```

```bash
$ cc crc16_comparison.c
$ ls -la

-rwxr-xr-x  1 xxxxxx  staff  33752 Aug 14 21:09 a.out
-rw-------  1 xxxxxx  staff   5927 Aug 14 21:07 crc16_comparison.c

$ ./a.out
```

Output of the program:

```
=== CRC-16 Implementation Comparison ===

Verification (10KB data):
  Table-based CRC: 0xE61A
  Algorithmic CRC: 0xE61A

=== Small Data (100 bytes) ===
Table-based:
  CRC Result: 0xC272
  Time: 0.685779 seconds
  Iterations: 1000000
  Time per iteration: 0.000000686 seconds
  Throughput: 139.06 MB/s

Algorithmic:
  CRC Result: 0xC272
  Time: 2.486196 seconds
  Iterations: 1000000
  Time per iteration: 0.000002486 seconds
  Throughput: 38.36 MB/s

=== Medium Data (10 KB) ===
Table-based:
  CRC Result: 0xE61A
  Time: 0.735653 seconds
  Iterations: 10000
  Time per iteration: 0.000073565 seconds
  Throughput: 129.64 MB/s

Algorithmic:
  CRC Result: 0xE61A
  Time: 5.258921 seconds
  Iterations: 10000
  Time per iteration: 0.000525892 seconds
  Throughput: 18.13 MB/s

=== Large Data (1 MB) ===
Table-based:
  CRC Result: 0x1357
  Time: 0.735148 seconds
  Iterations: 100
  Time per iteration: 0.007351480 seconds
  Throughput: 129.73 MB/s

Algorithmic:
  CRC Result: 0x1357
  Time: 5.935047 seconds
  Iterations: 100
  Time per iteration: 0.059350470 seconds
  Throughput: 16.07 MB/s
```

## Code Size

Using `cc` on OSX from XCode and with help of an utility called `bloaty` (install using `brew`), give the following size information:

```bash
$ bloaty -d symbols -n 50 a.out
```

```
SIZE FUNCTION    
--------------  
766  _main
541  _crc16_table
399  _benchmark
231  _crc16_algorithmic
171  _crc16_table_based

# other information omitted
```

## Summary

|Data Size|Table[s]  |Algorithmic[s]|Difference  
|---------|----------|--------------|----------|
| 100     | 0.685779 | 2.486196     | 3.625x   |
|  10kB   | 0.735653 | 5.258921     | 7.148x   |
|   1MB   | 0.735148 | 5.935047     | 8.073x   |

|CRC16 function | Code Size | Table Storage |
|---------------|-----------|---------------|
| Algorithmic   | 231 bytes | 0             |
| Table based   | 171 bytes | 541 bytes     |

## Source Code

```c
// crc16_comparison.c
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

// CRC-16-CCITT polynomial: 0x1021
#define CRC16_POLY 0x1021

// Pre-computed CRC-16 lookup table
static const uint16_t crc16_table[256] = {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
    0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
    0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
    0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
    0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
    0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
    0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
    0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
    0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
    0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
    0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
    0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
    0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
    0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
    0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
    0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
    0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
    0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
    0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
    0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
    0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
    0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
    0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};

// Table-based CRC-16 calculation
uint16_t crc16_table_based(const uint8_t *data, size_t length) {
    uint16_t crc = 0xFFFF;  // Initial value
    
    for (size_t i = 0; i < length; i++) {
        uint8_t index = (crc >> 8) ^ data[i];
        crc = (crc << 8) ^ crc16_table[index];
    }
    
    return crc;
}

// Algorithmic CRC-16 calculation
uint16_t crc16_algorithmic(const uint8_t *data, size_t length) {
    uint16_t crc = 0xFFFF;  // Initial value
    
    for (size_t i = 0; i < length; i++) {
        crc ^= (uint16_t)data[i] << 8;
        
        for (int bit = 0; bit < 8; bit++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ CRC16_POLY;
            } else {
                crc = crc << 1;
            }
        }
    }
    
    return crc;
}

// Benchmark function
void benchmark(const char *name, uint16_t (*crc_func)(const uint8_t*, size_t),
               const uint8_t *data, size_t length, int iterations) {
    clock_t start, end;
    double cpu_time_used;
    uint16_t crc = 0;
    
    start = clock();
    for (int i = 0; i < iterations; i++) {
        crc = crc_func(data, length);
    }
    end = clock();
    
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    printf("%s:\n", name);
    printf("  CRC Result: 0x%04X\n", crc);
    printf("  Time: %.6f seconds\n", cpu_time_used);
    printf("  Iterations: %d\n", iterations);
    printf("  Time per iteration: %.9f seconds\n", cpu_time_used / iterations);
    printf("  Throughput: %.2f MB/s\n\n", 
           (double)(length * iterations) / (cpu_time_used * 1024 * 1024));
}

int main(void) {
    // Test data sizes
    const size_t small_size  = 100;
    const size_t medium_size = 10000;
    const size_t large_size  = 1000000;
    
    uint8_t *small_data = malloc(small_size);
    uint8_t *medium_data = malloc(medium_size);
    uint8_t *large_data = malloc(large_size);
    
    // Initialize with pseudo-random data
    for (size_t i = 0; i < small_size; i++) small_data[i] = (uint8_t)(i * 7);
    for (size_t i = 0; i < medium_size; i++) medium_data[i] = (uint8_t)(i * 13);
    for (size_t i = 0; i < large_size; i++) large_data[i] = (uint8_t)(i * 17);
    
    printf("=== CRC-16 Implementation Comparison ===\n\n");
    
    // Verify both implementations produce same result
    uint16_t crc_table = crc16_table_based(medium_data, medium_size);
    uint16_t crc_algo = crc16_algorithmic(medium_data, medium_size);
    
    printf("Verification (10KB data):\n");
    printf("  Table-based CRC: 0x%04X\n", crc_table);
    printf("  Algorithmic CRC: 0x%04X\n", crc_algo);
    printf("  Match: %s\n\n", (crc_table == crc_algo) ? "YES" : "NO");
    
    // Benchmark small data
    printf("=== Small Data (100 bytes) ===\n");
    benchmark("Table-based", crc16_table_based, small_data, small_size, 1000000);
    benchmark("Algorithmic", crc16_algorithmic, small_data, small_size, 1000000);
    
    // Benchmark medium data
    printf("=== Medium Data (10 KB) ===\n");
    benchmark("Table-based", crc16_table_based, medium_data, medium_size, 10000);
    benchmark("Algorithmic", crc16_algorithmic, medium_data, medium_size, 10000);
    
    // Benchmark large data
    printf("=== Large Data (1 MB) ===\n");
    benchmark("Table-based", crc16_table_based, large_data, large_size, 100);
    benchmark("Algorithmic", crc16_algorithmic, large_data, large_size, 100);
    
    free(small_data);
    free(medium_data);
    free(large_data);
    
    return 0;
}
```