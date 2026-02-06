import matplotlib.pyplot as plt

# =========================================================
# HARDCODED DATA 
# Extracted from MT25020_Part_C_Results.csv provided by user
# =========================================================

# Shared X-Axis Data
msg_sizes = [1024, 4096, 16384, 65536]
thread_counts = [1, 2, 4, 8]
system_config = "System: Hybrid CPU (P-Core) | Linux Namespace"

# ---------------------------------------------------------
# DATA FOR PLOT 1: Throughput (Gbps) vs Message Size (Fixed Threads = 1)
# ---------------------------------------------------------
# A1 (Two-Copy) Rows: 1, 5, 9, 13
tp_A1 = [4.19, 15.47, 49.18, 75.24]
# A2 (One-Copy) Rows: 17, 21, 25, 29
tp_A2 = [4.17, 15.43, 49.94, 91.68]
# A3 (Zero-Copy) Rows: 33, 37, 41, 45
tp_A3 = [2.71, 9.28, 26.53, 50.64]

# ---------------------------------------------------------
# DATA FOR PLOT 2: Latency (us) vs Threads (Fixed MsgSize = 4096)
# ---------------------------------------------------------
# A1 (4096B) Rows: 5, 6, 7, 8
lat_A1 = [2.09, 4.23, 8.57, 17.68]
# A2 (4096B) Rows: 21, 22, 23, 24
lat_A2 = [2.09, 4.22, 8.65, 17.52]
# A3 (4096B) Rows: 37, 38, 39, 40
lat_A3 = [3.50, 7.04, 13.12, 26.38]

# ---------------------------------------------------------
# DATA FOR PLOT 3: LLC Misses vs Message Size (Fixed Threads = 1)
# ---------------------------------------------------------
# A1 Rows: 1, 5, 9, 13
llc_A1 = [34830, 58966, 302508, 37437]
# A2 Rows: 17, 21, 25, 29
llc_A2 = [10106, 6138, 10906, 7992]
# A3 Rows: 33, 37, 41, 45
llc_A3 = [36904, 18743, 19792, 7997]

# ---------------------------------------------------------
# DATA FOR PLOT 4: Efficiency (Raw Cycles & Throughput for Calculation)
# Fixed Threads = 1
# ---------------------------------------------------------
# Raw CPU Cycles from CSV (Threads=1)
cyc_A1 = [8624101244, 12836308565, 28227948684, 50890618159]
cyc_A2 = [11918848149, 15835309738, 28537316155, 50893477627]
cyc_A3 = [40120955807, 33386976129, 50407388328, 50891927966]

# =========================================================
# PLOTTING FUNCTIONS
# =========================================================

def plot_throughput():
    plt.figure(figsize=(10, 6))
    plt.plot(msg_sizes, tp_A1, marker='o', linestyle='-', label="A1 (Two-Copy)")
    plt.plot(msg_sizes, tp_A2, marker='s', linestyle='-', label="A2 (One-Copy)")
    plt.plot(msg_sizes, tp_A3, marker='^', linestyle='-', label="A3 (Zero-Copy)")
    
    plt.title(f"Throughput vs Message Size (1 Thread)\n{system_config}")
    plt.xlabel("Message Size (Bytes)")
    plt.ylabel("Throughput (Gbps)")
    plt.xscale('log', base=2)
    plt.grid(True, which="both", ls="-", alpha=0.5)
    plt.legend()
    
    filename = "MT25020_Throughput_vs_MsgSize.png"
    plt.savefig(filename)
    print(f"Generated {filename}")
    plt.close()

def plot_latency():
    plt.figure(figsize=(10, 6))
    plt.plot(thread_counts, lat_A1, marker='o', linestyle='-', label="A1 (Two-Copy)")
    plt.plot(thread_counts, lat_A2, marker='s', linestyle='-', label="A2 (One-Copy)")
    plt.plot(thread_counts, lat_A3, marker='^', linestyle='-', label="A3 (Zero-Copy)")
    
    plt.title(f"Latency vs Thread Count (MsgSize=4096)\n{system_config}")
    plt.xlabel("Thread Count")
    plt.ylabel("Latency (µs)")
    plt.grid(True, ls="-", alpha=0.5)
    plt.legend()
    
    filename = "MT25020_Latency_vs_Threads.png"
    plt.savefig(filename)
    print(f"Generated {filename}")
    plt.close()

def plot_cache_misses():
    plt.figure(figsize=(10, 6))
    plt.plot(msg_sizes, llc_A1, marker='o', linestyle='-', label="A1 (Two-Copy)")
    plt.plot(msg_sizes, llc_A2, marker='s', linestyle='-', label="A2 (One-Copy)")
    plt.plot(msg_sizes, llc_A3, marker='^', linestyle='-', label="A3 (Zero-Copy)")
    
    plt.title(f"LLC Misses vs Message Size (1 Thread)\n{system_config}")
    plt.xlabel("Message Size (Bytes)")
    plt.ylabel("LLC Misses (Events)")
    plt.xscale('log', base=2)
    plt.yscale('log') # Log scale helps visualize the differences
    plt.grid(True, ls="-", alpha=0.5)
    plt.legend()
    
    filename = "MT25020_CacheMisses_vs_MsgSize.png"
    plt.savefig(filename)
    print(f"Generated {filename}")
    plt.close()

def plot_efficiency():
    plt.figure(figsize=(10, 6))
    DURATION = 10.0
    
    # Calculate Efficiency: Cycles / TotalBytes
    # TotalBytes = (Throughput_Gbps * 1e9 * Duration) / 8
    
    eff_A1 = []
    for c, t in zip(cyc_A1, tp_A1):
        total_bytes = (t * 1e9 * DURATION) / 8.0
        eff_A1.append(c / total_bytes if total_bytes > 0 else 0)

    eff_A2 = []
    for c, t in zip(cyc_A2, tp_A2):
        total_bytes = (t * 1e9 * DURATION) / 8.0
        eff_A2.append(c / total_bytes if total_bytes > 0 else 0)

    eff_A3 = []
    for c, t in zip(cyc_A3, tp_A3):
        total_bytes = (t * 1e9 * DURATION) / 8.0
        eff_A3.append(c / total_bytes if total_bytes > 0 else 0)
    
    plt.plot(msg_sizes, eff_A1, marker='o', linestyle='-', label="A1 (Two-Copy)")
    plt.plot(msg_sizes, eff_A2, marker='s', linestyle='-', label="A2 (One-Copy)")
    plt.plot(msg_sizes, eff_A3, marker='^', linestyle='-', label="A3 (Zero-Copy)")
    
    plt.title(f"CPU Efficiency (Cycles per Byte)\n{system_config}")
    plt.xlabel("Message Size (Bytes)")
    plt.ylabel("Cycles per Byte (Lower is Better)")
    plt.xscale('log', base=2)
    # plt.yscale('log') # Optional
    plt.grid(True, ls="-", alpha=0.5)
    plt.legend()
    
    filename = "MT25020_CPUCycles_vs_MsgSize.png"
    plt.savefig(filename)
    print(f"Generated {filename}")
    plt.close()

if __name__ == "__main__":
    try:
        plot_throughput()
        plot_latency()
        plot_cache_misses()
        plot_efficiency()
        print("\nSUCCESS")
    except Exception as e:
        print(f"Error generating plots: {e}")