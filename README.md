# php-keccak256

[![PHP Version](https://img.shields.io/badge/PHP-8.1%2B-blue.svg)](https://php.net)
![License](https://img.shields.io/badge/License-MIT-green.svg)
[![Version](https://img.shields.io/badge/Version-1.0.0-orange.svg)](https://github.com/RandomCoderTinker/php-keccak256/releases)

A lightweight PHP extension that provides **Ethereum-compatible Keccak-256** hashing (padding suffix `0x01`) with minimal overhead.  
_This is **not** the NIST SHA3-256 (`0x06`) variant. Use `hash('sha3-256', …)` for standard SHA3-256._

---

## Features

- **Keccak-256 only** (Ethereum style, `0x01` padding)  
- **Raw or hex output**: 32-byte binary blob or 64-character hex string  
- **Blazing fast**: C wrapper outperforms pure-PHP implementations by ~17×  

---

## Standards

| Function                              | Padding | Purpose                           |
|---------------------------------------|:-------:|-----------------------------------|
| `keccak_hash()` (this extension)      | `0x01`  | Ethereum, smart contracts, dApps  |
| `hash('sha3-256', …)` (PHP built-in)  | `0x06`  | NIST SHA3-256 standard            |

## Installation

### Prerequisites

- PHP 8.1 or later
- PHP development headers and tools (`php-dev`, `phpize`, `autoconf`, `make`, `gcc`)

On Debian/Ubuntu:
```bash
sudo apt-get update
sudo apt-get install -y php-dev autoconf make gcc
````

### Build & Install

1. Clone the repo:

   ```bash
   git clone https://github.com/RandomCoderTinker/php-keccak256.git
   cd php-keccak256
   ```
2. Prepare the build:

   ```bash
   phpize
   ./configure
   ```
3. Compile and install:

   ```bash
   make
   sudo make install
   ```
4. Enable the extension by adding to your `php.ini`:

   ```ini
   extension=keccak.so
   ```
5. Restart your web server or PHP-FPM:

   ```bash
   sudo service apache2 restart
   # or
   sudo service php8.x-fpm restart
   ```

## Usage

```php
<?php
$data = "hello world";

// Hexadecimal output (default)
$hex = keccak_hash($data);
echo $hex; // e.g. d4ee...a3f

// Raw binary output
$raw = keccak_hash($data, true);
echo bin2hex($raw); // same hex representation
```

## Performance Comparison

| Implementation                      | Hex Output | Raw Output |
| ----------------------------------- | ---------: | ---------: |
| **C extension (`keccak_hash`)**     |   3.119 ms |   2.902 ms |
| **KornRunner PHP (`Keccak::hash`)** |  52.729 ms |  50.877 ms |

> Benchmarked over 100 iterations on identical hardware.
> The C extension delivers a \~17× speedup over the pure-PHP KornRunner package.

## Why Keccak-256 Only?

Many applications—especially blockchain clients, Ethereum tooling, and cryptographic protocols—rely exclusively on the 256-bit Keccak (SHA-3) digest:

* **Standardization**: Ethereum and many smart-contract platforms specify Keccak-256 as the sole hash function.
* **Simplicity**: If you don’t need SHA-3-224, SHA-3-384, SHA-3-512, or other ciphers, a focused extension lowers complexity, binary size, and attack surface.
* **Performance**: A minimal implementation optimizes for one output size; there’s no overhead handling multiple rates or capacities.

By offering just Keccak-256, this extension provides a drop-in, high-performance hash function tailored to the vast ecosystem of applications that only require the 256-bit variant.

---
Built with ❤️ using PHP 8.1+
