# Network Throughput & Latency Benchmarking (Socket I/O Optimization)

---

## 1. Project Overview
This project benchmarks three different approaches to sending data over a TCP socket to analyze the impact of **User-to-Kernel data copying** on network throughput, latency, and CPU efficiency.

The three implementations are:
* **A1 (Two-Copy):** The traditional approach. Data is copied from multiple struct fields into a single linear user-space buffer (`memcpy`), then sent to the kernel using `send()`.
* **A2 (One-Copy / Scatter-Gather):** Optimized approach. Uses `sendmsg()` with an `iovec` array to pass pointers directly to the kernel, eliminating the user-space `memcpy`.
* **A3 (Zero-Copy):** Advanced approach. Uses `sendmsg()` with the `MSG_ZEROCOPY` flag to instruct the kernel to pin pages and avoid copying data into kernel space (requires OS support).

The goal is to measure **Throughput (Gbps)**, **Latency (µs)**, and **CPU Metrics** (Cycles, Cache Misses) inside a controlled Linux Network Namespace environment.

---

## 2. File Structure

| File Name | Description |
| :--- | :--- |
| `MT25020_Part_A1_Server.c` | Server implementation for Two-Copy (Standard `send`). |
| `MT25020_Part_A1_Client.c` | Client implementation for Two-Copy. |
| `MT25020_Part_A2_Server.c` | Server implementation for One-Copy (`sendmsg` / Scatter-Gather). |
| `MT25020_Part_A2_Client.c` | Client implementation for One-Copy. |
| `MT25020_Part_A3_Server.c` | Server implementation for Zero-Copy (`MSG_ZEROCOPY`). |
| `MT25020_Part_A3_Client.c` | Client implementation for Zero-Copy. |
| `MT25020_Common.h` | Shared header file defining message structures and constants. |
| `MT25020_Part_C_RunExperiments.sh` | Bash script to setup namespaces, run `perf`, and log results. |
| `MT25020_Part_D_Plots.py` | Python script to generate graphs (Hardcoded data arrays). |
| `Makefile` | Script to compile all server and client executables. |
| `MT25020_Part_C_Results.csv` | Output file containing raw benchmark data. |

---

## 3. Prerequisites
Ensure the following tools are installed on the Linux machine:
* **GCC Compiler:** `sudo apt install build-essential`
* **Perf Tool:** `sudo apt install linux-tools-common linux-tools-generic`
* **Python 3 & Matplotlib:** `sudo apt install python3-matplotlib`

---

## 4. How to Run the Experiments

### Step 1: Compile the Code
Use the Makefile to compile all 6 executables (Server/Client for A1, A2, A3).
```bash
make clean
make all
```

### Step 2: Run the Benchmarking Script
The bash script automates the entire process:

* Sets up virtual network namespaces (ns_server, ns_client) connected via veth pairs.

* Runs the Server in the background (pinned to P-Core).

* Attaches perf to the Server process to measure CPU cycles and cache misses.

* Runs the Client to generate load.

* Saves all metrics to MT25020_Part_C_Results.csv.

* Run with sudo privileges:

```bash

sudo ./MT25020_Part_C_RunExperiments.sh
```

### 5. Generating Plots
The plotting script is standalone and contains the hardcoded data from the best experimental run (as per rubric requirements). It generates 4 plots:

* Throughput vs Message Size

* Latency vs Thread Count

* LLC Misses vs Message Size

* CPU Efficiency (Cycles/Byte)

To generate plots:

```bash

python3 MT25020_Part_D_Plots.py

```

Note: If you encounter a "Permission Denied" error, run with sudo or delete old .png files first.

### 6. Key Observations
A2 (One-Copy) is the Winner:

* For large message sizes (64KB), A2 achieves the highest throughput (~92 Gbps).

* It effectively eliminates the user-space copy cost without incurring the high setup overhead of Zero-Copy.

A3 (Zero-Copy) Overhead:

* Zero-Copy (MSG_ZEROCOPY) performs worse than standard copying for small messages (< 16KB).

* The overhead of page pinning and handling the error queue in the kernel is higher than a simple CPU memcpy for small data.

Latency & Threads:

* As thread count increases (1 -> 8), latency increases significantly (e.g., 2µs -> 17µs).

* This confirms expected behavior due to CPU contention and context switching overhead.

### 7. Troubleshooting
* "Permission Denied" on Plots: If the script cannot overwrite images, delete them manually: rm *.png.

* "Perf not found": Ensure linux-tools matching your kernel version is installed.

* Zero Throughput: Ensure namespaces are cleaned up: sudo ip netns del ns_server.
