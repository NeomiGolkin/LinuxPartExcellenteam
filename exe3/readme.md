# Simple Shell

A custom Unix shell implemented in C++ as part of systems programming coursework. This project explores low-level process management, system calls, environment variable parsing, and asynchronous background job execution.

---

## Features

- **Built-in Commands:**
  - `cd`: Changes the current working directory, including full support for relative paths, `..` (parent directory), and `.` (current directory).
  - `pwd`: Prints the absolute path of the current working directory.
  - `myjobs`: Displays all active and completed background processes along with their PIDs and statuses (`RUNING` / `DONE`).
  - `exit`: Terminates the shell session.
- **External Command Execution:**
  - Executes external binaries by searching the system `PATH` environment variable or utilizing direct paths (without relying on `execvp`).
  - Implements process control using `fork()`, `execve()`, and `waitpid()`.
- **Background Processes:**
  - Supports running tasks asynchronously by appending `&` to the command line (e.g., `sleep 10 &`), maintaining non-blocking tracking and status updates.

---

## Project Structure

```text
exe3 partA/
├── main.cpp          # Core C++ implementation of the shell logic
├── CMakeLists.txt    # Build configuration file for CMake
└── build/            # Build directory (generated during compilation)