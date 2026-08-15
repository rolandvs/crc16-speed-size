# crc16-speed-size
This repository is about the use of CRCs in embedded software.

# CRC
[Cyclic redundancy check](https://en.wikipedia.org/wiki/Cyclic_redundancy_check) (CRC) was first proposed by W. Peterson and D.T. Brown in _"Cyclic Codes for Error Detection," Proceedings of the IRE 49(1), January 1961, pp. 228–235_, as a method for error detection in communication networks. It utilizes polynomial division to generate a check value that helps identify accidental changes to digital data. CRC-32 as used in Ethernet came later, mid-1970s.

For some more implementations of CRCs in a nice library, visit Lammert Bies [github](https://github.com/lammertb/libcrc) repo.

# An overview of different CRC16 algorithms and their impact
CRC's are used in all kind of applications like verification, integrity checks, etc. Although CRC-8 and CRC-32 are also commonly used, we use CRC-16 to illustrate the speed and size optimalisation/penalty when using it.

For PCs, Desktop and Laptops a slower calculation is not a huge problem, unless lots of data need to be processed. Memory is not a big issue when using tables and otherwise the computational speed is high enough to keep up when used in communication.

For embedded controllers it is different as memory is usually much more constrained and clock speeds much lower than desktop machines. We go from Gigabytes to kilobytes and from Gigahertz to Megahertz...

