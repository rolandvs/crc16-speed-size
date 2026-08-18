# CRC16 Size and Speed Part 2

In the previous part we have seen the speed and size of different CRC-16 implementations. In this part we will see how to use the Hardware CRC unit on STM32 microcontrollers.

Also added in the repository is a program [crcgen.c](src/crcgen.c) to generate the CRC table based upon a polynominal you enter. If you use multiple protocols within one program like MODBUS and CCITT you may end up with multiple tables due to the difference of the polynominal. So if you are using the STM32, use the Hardware CRC, which saves space and time.

# CRC16 using STM32 Hardware CRC

The F1, F2, F4 series of microcontrollers have a CRC unit that is fixed on using 32-bit and have a fixed polynominal value. The other processors can be reprogrammed like this example.

The compiled program inherits the test on CRC-16 using full and nibble sized tables and the algorithmic implementation along with a byte fed or word fed **Hardware CRC**.

The HW CRC is the fastest and uses less energy. It is available on all STM32 micro-controllers, so why not use it!?

## Quick Quick Slow

In a first test the Hardware CRC (word) came out worse (561ms) than full table CRC. This was not due the hardware accelerator, but the code that was feeding the bytes. The original slow code is still in the source, but disabled and replaced with somthing that is faster.

## Test Results

When the STM32 project is compiled and loaded into the target device the following  output is generated:

```
=== CRC-16 on STM32F091RC @ 48MHz ===

align=0  len%4=1
byte: 29B1 (expect 29B1)
word: 29B1 (expect 29B1)

# depending on the compiler flag `-Og` used to compile

=== CRC-16 Benchmark Results (STM32F091RC @ 48MHz) ===

Full table (512 bytes)    CRC: 0x3AEA  Time:   439 ms  Speed: 2277904 bytes/s
Nibble table (32 bytes)   CRC: 0x3AEA  Time:   689 ms  Speed: 1451378 bytes/s
Algorithmic (0 bytes)     CRC: 0x3AEA  Time:  2778 ms  Speed:  359971 bytes/s
Hardware CRC (byte)       CRC: 0x3AEA  Time:   149 ms  Speed: 6711409 bytes/s
Hardware CRC (word)       CRC: 0x3AEA  Time:    42 ms  Speed: 23809523 bytes/s


# depending on the compiler flag `-O2` used to compile

=== CRC-16 Benchmark Results (STM32F091RC @ 48MHz) ===

Full table (512 bytes)    CRC: 0x3AEA  Time:   376 ms  Speed: 2659574 bytes/s
Nibble table (32 bytes)   CRC: 0x3AEA  Time:   585 ms  Speed: 1709401 bytes/s
Algorithmic (0 bytes)     CRC: 0x3AEA  Time:  2319 ms  Speed:  431220 bytes/s
Hardware CRC (byte)       CRC: 0x3AEA  Time:   123 ms  Speed: 8130081 bytes/s
Hardware CRC (word)       CRC: 0x3AEA  Time:    36 ms  Speed: 27777777 bytes/s
```

The code size comparison can be derived from:

```bash
$ arm-none-eabi-nm --print-size --size-sort --radix=d build/crc16.elf | grep -i "crc"
```

# Speed and Size Comparison

Embedded C code on a STM32F091RC running at 48MHz.

| CRC16 function | Code Size | Table Storage | Speed  |
|----------------|-----------|---------------|--------|
| Algorithmic    |  72 bytes |   0           | 2319ms |
| Table based    |  52 bytes | 512 bytes     |  376ms |
| Nibble table   |  72 bytes |  32 bytes     |  585ms |
| HW CRC (byte)  |  28 bytes |   0           |  123ms |
| HW CRC (word)  | 192 bytes |   0           |   36ms |

Initialisation and setup code is not counted, only the functions.
