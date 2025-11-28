# xe_xerces_example

[한국어 README](README.ko.md)

## Overview
This project demonstrates how to validate an XML file against an XSD schema using **Xerces-C++** and **spdlog**.

## Features
- XML validation using Xerces-C++
- Custom SAX2 error handler
- Logging through spdlog
- Cross‑platform CMake configuration (MSVC, MinGW, Linux)

## File Structure
- `main.cpp` – entry point  
- `xml_validator_app.*` – validation controller  
- `simple_error_handler.*` – SAX2 error handler  
- `person_int_schema.xsd` – sample schema  
- `person_invalid.xml` – intentionally invalid sample XML  

## Build
```bash
cmake -B build
cmake --build build
```

## How it Works
1. Loads XML / XSD paths (from project root)
2. Initializes Xerces‑C++
3. Configures SAX2 parser with schema checking
4. Collects validation errors

