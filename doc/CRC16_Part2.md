# CRC16 using STM32 Hardware CRC

The F1, F2, F4 series of microcontrollers have a CRC unit that is fixed on using 32-bit and have a fixed polynominal value. The other processors can be reprogrammed like this example.

The compiled program inherits the test on CRC16 using full and nibble sized tables and the algorithmic implementation along with a byte fed or word fed HW CRC.

The HW CRC is the fastest and uses less energy. It is available on all STM32 micro-controllers, so why not use it!?


```
=== CRC-16 on STM32F091RC @ 48MHz ===

align=0  len%4=1
byte: 29B1 (expect 29B1)
word: 29B1 (expect 29B1)

# compiler flag `-Og`

=== CRC-16 Benchmark Results (STM32F091RC @ 48MHz) ===

Full table (512 bytes)    CRC: 0x3AEA  Time:   439 ms  Speed: 2277904 bytes/s
Nibble table (32 bytes)   CRC: 0x3AEA  Time:   689 ms  Speed: 1451378 bytes/s
Algorithmic (0 bytes)     CRC: 0x3AEA  Time:  2778 ms  Speed:  359971 bytes/s
Hardware CRC (byte)       CRC: 0x3AEA  Time:   149 ms  Speed: 6711409 bytes/s
Hardware CRC (word)       CRC: 0x3AEA  Time:    42 ms  Speed: 23809523 bytes/s


# compiler flag `-O2`
=== CRC-16 Benchmark Results (STM32F091RC @ 48MHz) ===

Full table (512 bytes)    CRC: 0x3AEA  Time:   376 ms  Speed: 2659574 bytes/s
Nibble table (32 bytes)   CRC: 0x3AEA  Time:   585 ms  Speed: 1709401 bytes/s
Algorithmic (0 bytes)     CRC: 0x3AEA  Time:  2319 ms  Speed:  431220 bytes/s
Hardware CRC (byte)       CRC: 0x3AEA  Time:   123 ms  Speed: 8130081 bytes/s
Hardware CRC (word)       CRC: 0x3AEA  Time:    36 ms  Speed: 27777777 bytes/s
```
