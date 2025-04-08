# WalletCache

## Overview
- WalletCache is a CLI app written in modern, cross-platform C++ with the goal of being a **secure, lightweight, and performant** means of storing credit cards digitally. 
- Leverages industry best practices in unit testing with GoogleTest, mocking with gMock, dependency injection, and CI/CD workflows, ensuring code correctness and efficient development.
- Utilizes the widely-trusted libsodium library alongside rigorous secure coding standards and robust UX design, guaranteeing data integrity and program security
- Incorporates optimizations such as lazy loading and efficient low-level I/O code to provide performant operation.

## Usage
WalletCache is designed with ease of use and security in mind. For example, secure data is hidden by default yet can easily be copied to clipboard with the press of a key.

To use, you'll need to obtain the binary. It can be built from source by running `cmake --build build` in the root directory; alternatively, pre-build releases are available in the repository's **Releases** section

Once a binary is acquired, run `/path/to/binary/WalletCache` in your terminal emulator of choice to open up the program. 

![WalletCache Logo](logo.jpeg?raw=true "WalletCache Logo")
