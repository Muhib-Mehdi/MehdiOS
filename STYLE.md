# Coding Style Guide

## General Principles
- **Clarity over cleverness**: Write code that is easy to read.
- **Explain WHY, not WHAT**: Comments should explain the reasoning, not just repeat the code.
- **Consistency**: Follow the existing patterns.

## C Code
- **Indentation**: 4 spaces. No tabs.
- **Naming**:
  - Functions: `snake_case` (e.g., `clear_screen`, `port_byte_in`)
  - Variables: `snake_case` (e.g., `video_memory`)
  - Constants: `UPPER_CASE` (e.g., `MAX_ROWS`)
  - Types: `snake_case_t` (e.g., `uint32_t`)
- **Braces**: Opening brace on the same line as the statement.
  ```c
  if (condition) {
      // code
  }
  ```
- **Headers**: Every `.c` file must have a corresponding `.h` file in `include/`.

## Assembly Code
- **Indentation**: Instructions indented by 4 spaces. Labels at column 0.
- **Comments**: Align comments for readability.
  ```nasm
  global _start

  _start:
      mov ax, 0x01    ; Load AX
  ```

## Repository Structure
- `boot/`: Bootloader assembly.
- `kernel/`: Core kernel logic.
- `drivers/`: Hardware drivers.
- `include/`: Share header files.
- `docs/`: Technical documentation.
