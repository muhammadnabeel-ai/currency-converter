# Currency Converter Application

## Description

A modular C++ command-line application that performs real-time currency conversions, manages exchange rates via file I/O, records transaction history, and maintains admin activity logs. Built using Object-Oriented Programming (OOP) concepts in C++.

## Features

- **Real-Time Currency Conversion**: Convert amounts between supported international currencies using USD as a standard baseline.
- **Dynamic File I/O Storage**: Loads exchange rates dynamically from `rates.txt` and currency metadata from `currency_list.txt`.
- **Transaction History Logging**: Keeps a record of user conversion transactions in `conversion_history.txt`.
- **Admin Logging Utility**: Tracks system activities and administrative actions in `admin_log.txt`.
- **OOP Architecture**: Leverages classes for encapsulation (`Logger`, `Currency`), clean separation of concerns, and structured error handling.

## Concepts Used

- **C++ Standard Library**: `<iostream>`, `<fstream>`, `<iomanip>`, `<ctime>`
- **Object-Oriented Programming (OOP)**: Encapsulation, static loggers, constructor initialization.
- **File I/O**: Stream processing (`ofstream`, `ifstream`) with append modes for logging.
- **Date & Time Processing**: Timestamps for conversion transactions and system logs.

## Build and Run

### Prerequisites
- GCC/G++ Compiler (MinGW for Windows, or native Linux/macOS C++ compiler).

### Compilation
Open your terminal in the project directory and run:

```bash
g++ currency_converter.cpp -o currency_converter