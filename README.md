# mdadm Linear Devices writing

# Academic Integrity Statement

Please note that all work included in this project is the original work of the author, and any external sources or references have been properly cited and credited. It is strictly prohibited to copy, reproduce, or use any part of this work without permission from the author.

If you choose to use any part of this work as a reference or resource, you are responsible for ensuring that you do not plagiarize or violate any academic integrity policies or guidelines. The author of this work cannot be held liable for any legal or academic consequences resulting from the misuse or misappropriation of this work.

Any unauthorized copying or use of this work may result in serious consequences, including but not limited to academic penalties, legal action, and damage to personal and professional reputation. Therefore, please use this work only as a reference and always ensure that you properly cite and attribute any sources or references used.

---


## Overview
This project implements the `mdadm_write` function as part of the mdadm storage system. The `mdadm_write` function is responsible for writing data to the storage system at a specified address. The storage system unifies multiple small disks under a single address space, allowing read and write operations.

## Program Functionality
The `mdadm_write` function has the following signature:
```c
int mdadm_write(uint32_t addr, uint32_t len, const uint8_t *buf);
```
- `addr`: The starting address where data should be written.
- `len`: The length of the data to be written (up to 1,024 bytes).
- `buf`: A pointer to the buffer containing the data to be written (read-only).

The function ensures that data is written to the correct address within the storage system and enforces certain restrictions, such as a maximum length of 1,024 bytes and boundary checks to prevent out-of-bounds writes.

## Software Setup
Before running the program, you need to set up the necessary environment and dependencies.

1. **Clone the Repository:**
   ```
   git clone https://github.com/yourusername/mdadm-Linear-Devices-writing-and-testing.git
   cd mdadm-Linear-Devices-writing-and-testing
   ```

2. **Compile the Program:**
   ```
   gcc -o mdadm mdadm.c
   ```

## Running the Program
After setting up the software, you can run the program with different modes:

### Unit Tests
To run the unit tests, execute the program without any arguments:
```
./mdadm_write
```

This mode will perform unit tests to check the correctness of the `mdadm_write` function.

### Trace Replay
To test your implementation using trace files, use the following command:
```
./mdadm_write -w traces/trace-file-name
```
Replace `trace-file-name` with one of the available trace files, such as `simple-input`, `linear-input`, or `random-input`. These trace files contain a sequence of commands for the storage system.


## Notes
- Ensure that you have all necessary permissions to execute the program and access the trace files.
- Refer to the provided trace files and the accompanying documentation for more details on how to interpret and use them.
- Always adhere to academic integrity guidelines and avoid unauthorized use or sharing of this work.

If you encounter any issues or have questions, please contact the project author for assistance.