# MyFS - Simple Filesystem Simulator

## Project Objective
This project implements a simple filesystem operating in userspace within a Linux environment. The system simulates classic filesystem layers (VFS, Filesystem, Block Device) and allows performing basic file management operations via a Command Line Interface (CLI). The physical storage device is simulated using a file on the host system, ensuring data persistence between executions.

## How to Compile and Run
To build and run the simulator, open a terminal in the project's root directory and execute the following commands:

```bash
mkdir build
cd build
cmake ..
make
./myfs_app /tmp/myfs_storage.bin
