# Fortran ZIP Bomb Educational Project

## ⚠️ Security Warning
**This project is for educational and research purposes only!**
- Do not use for malicious attacks on websites or systems
- Ensure testing in a secure environment before use
- Generated files may consume significant disk space and memory
- Please comply with relevant laws, regulations, and ethical standards

## Project Overview

This is a multi-language collaborative educational demonstration project showcasing how to create a ZIP bomb generator using **Fortran + C++ + C**. The project demonstrates:

- Modern Fortran and C/C++ hybrid programming
- File compression algorithm implementation principles
- System-level programming concepts
- Compression bomb attack principles in cybersecurity

## How It Works

```
Small file (few KB) → Decompressed → Huge file (10GB+)
```

By creating highly repetitive data patterns and leveraging compression algorithm characteristics, it achieves an extremely high compression ratio. When the target system decompresses, it consumes substantial resources.

## Environment Requirements

### Required Software
- **VSCode** + Modern Fortran extension
- **gfortran** (GNU Fortran compiler)
- **g++** (GNU C++ compiler)
- **gcc** (GNU C compiler)
- **make** build tool

### Recommended Setup
```bash
# macOS (using Homebrew)
brew install gcc make

# Linux (Ubuntu/Debian)
sudo apt install gfortran g++ gcc make

# Verify installation
gfortran --version
g++ --version
gcc --version
```

## Quick Start

### 1. Clone Repository
```bash
git clone https://github.com/ktwu01/Fortran-Playground.git
cd Fortran-Playground
```

### 2. Compile Project
```bash
make zipbomb
```

### 3. Run Program
```bash
./zipbomb
```

The program will generate a file named `bomb.zip` (~5KB) that expands to approximately 10GB when decompressed.

## Project Structure

```
Fortran-Playground/
├── src/
│   ├── main.f90           # Fortran main program
│   ├── zipbomb.cpp        # C++ ZIP generation module
│   ├── utils.c            # C system utility functions
│   └── interfaces.f90     # Fortran-C interface definitions
├── include/
│   └── zipbomb.h          # C/C++ header file
├── Makefile               # Build script
└── README.md             # Project documentation
```

## Core Code Analysis

### Fortran Main Controller (main.f90)
```fortran
program zipbomb_generator
    use iso_c_binding
    implicit none
    
    ! Call C++ function to generate ZIP bomb
    call create_zipbomb(c_char_"bomb.zip"//c_null_char)
    
    write(*,*) '⚠️  ZIP bomb generated: bomb.zip'
    write(*,*) 'File size: ~5KB'
    write(*,*) 'Decompressed size: ~10GB'
    write(*,*) 'Please test in a safe environment!'
end program
```

### C++ Compression Engine (zipbomb.cpp)
```cpp
#include <iostream>
#include <fstream>
#include <string>

extern "C" {
    void create_zipbomb(const char* filename);
}

void create_zipbomb(const char* filename) {
    // Generate highly repetitive data
    std::string pattern(1024*1024, 'A'); // 1MB repeated characters
    
    // Create ZIP file structure
    // Leverage ZIP format's repetitive data compression features
    // Achieve small file → large decompression effect
}
```

### C System Interface (utils.c)
```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

// Get file size
long get_file_size(const char* filename) {
    struct stat st;
    if (stat(filename, &st) == 0) {
        return st.st_size;
    }
    return -1;
}
```

## Compilation Options Explained

```makefile
# Makefile core configuration
FC = gfortran
CC = gcc  
CXX = g++

# Compilation flags
FFLAGS = -std=f2008 -Wall -Wextra -g
CFLAGS = -Wall -Wextra -g
CXXFLAGS = -std=c++17 -Wall -Wextra -g

# Link multi-language object files
zipbomb: main.o zipbomb.o utils.o
	$(FC) $(FFLAGS) -o $@ $^ -lstdc++
```

## Learning Points

### 1. Multi-Language Interoperability
- Fortran calls C functions through `iso_c_binding`
- C++ provides advanced file processing capabilities
- C handles low-level system calls

### 2. Compression Algorithm Principles
- Repetitive data detection
- Dictionary compression techniques
- Compression ratio optimization strategies

### 3. System Programming Concepts
- File I/O operations
- Memory management
- Cross-platform compatibility

## Testing and Verification

### Safe Testing Environment
```bash
# Create test directory
mkdir test_env
cd test_env

# Generate ZIP bomb
../zipbomb

# Check file size
ls -lh bomb.zip

# ⚠️ Cautious decompression test
# unzip bomb.zip  # Warning: Will generate large files!
```

### Performance Analysis
- Compression ratio calculation
- Generation time measurement
- Memory usage monitoring

## Troubleshooting

### Common Issues

**Compilation error: gfortran not found**
```bash
# Check compiler path
which gfortran
# If not found, reinstall gcc suite
```

**Link error: undefined reference**
```bash
# Check library linking order
# Ensure -lstdc++ is at the end
```

**Runtime error: insufficient permissions**
```bash
# Ensure write permissions
chmod +w .
```

## Extended Features

### Advanced Implementation
- Support for different compression formats (7z, RAR)
- Configurable compression parameters
- Batch generation functionality
- Graphical interface

### Performance Optimization
- Parallel compression algorithms
- Memory-mapped files
- Stream processing

## Related Resources

### Learning Materials
- [Modern Fortran Tutorial](https://fortran-lang.org/)
- [C++17 Standard Documentation](https://en.cppreference.com/)
- [ZIP File Format Specification](https://pkware.cachefly.net/webdocs/casestudies/APPNOTE.TXT)

### Development Tools
- [VSCode Fortran Extension](https://marketplace.visualstudio.com/items?itemName=fortls.fortls)
- [GNU Compiler Collection](https://gcc.gnu.org/)

## Contributing Guidelines

Pull Requests and Issues are welcome!

### Development Standards
- Code comments in English
- Follow standard style for each language
- Add appropriate error handling
- Write test cases

## License

This project is for educational purposes only. Do not use for illegal purposes.

---

**Reminder: This tool is for learning and research only. Please use in a safe environment!**