# MT25020_Part_D_Plots.py
import matplotlib.pyplot as plt
import numpy as np

# Set global style parameters
plt.rcParams['figure.figsize'] = (10, 6)
plt.rcParams['font.size'] = 12

# ==========================================
# PART A: Throughput vs Message Size
# ==========================================
print("Generating Plot 1: Throughput vs Message Size...")

# Hardcoded data from CSV - Throughput vs Message Size (averaged across threads)
message_sizes = [1024, 4096, 16384, 65536]

# A1 Implementation - Average throughput for each message size
a1_throughput = [
    (1.813721 + 1.743771 + 1.901260 + 1.654298) / 4,  # 1024 bytes
    (3.748368 + 3.641640 + 3.457383 + 2.334435) / 4,  # 4096 bytes
    (7.562831 + 7.290157 + 5.714884 + 4.049747) / 4,  # 16384 bytes
    (24.683154 + 23.371526 + 19.276332 + 16.154771) / 4  # 65536 bytes
]

# A2 Implementation
a2_throughput = [
    (5.053669 + 4.651849 + 3.865997 + 3.647873) / 4,
    (11.239933 + 9.677922 + 8.600654 + 5.486799) / 4,
    (38.737563 + 35.183268 + 28.729002 + 24.645704) / 4,
    (82.359073 + 74.190916 + 60.213093 + 49.393928) / 4
]

# A3 Implementation
a3_throughput = [
    (3.328083 + 2.885124 + 2.449172 + 1.739205) / 4,
    (8.353762 + 7.646930 + 6.393183 + 5.256072) / 4,
    (27.292126 + 25.140473 + 20.733090 + 17.943326) / 4,
    (60.141648 + 55.080877 + 46.034261 + 29.876765) / 4
]

plt.figure()
plt.plot(message_sizes, a1_throughput, marker='o', label='A1 (Two-Copy)', linewidth=2)
plt.plot(message_sizes, a2_throughput, marker='s', label='A2 (One-Copy)', linewidth=2)
plt.plot(message_sizes, a3_throughput, marker='^', label='A3 (Zero-Copy)', linewidth=2)

plt.xlabel('Message Size (bytes)', fontsize=12)
plt.ylabel('Throughput (Gbps)', fontsize=12)
plt.title('Throughput vs Message Size', fontsize=14, fontweight='bold')
plt.xscale('log', base=2)
plt.grid(True, alpha=0.3)
plt.legend(fontsize=10)
plt.tight_layout()
plt.savefig('MT25020_Part_D_Throughput_vs_MessageSize.png', dpi=300)
print("Saved: MT25020_Part_D_Throughput_vs_MessageSize.png")

# ==========================================
# PART B: Latency vs Thread Count
# ==========================================
print("Generating Plot 2: Latency vs Thread Count...")

# Hardcoded data from CSV - Latency vs Thread Count (averaged across message sizes)
thread_counts = [1, 2, 4, 8]

# A1 Implementation - Average latency for each thread count
a1_latency = [
    (4.516687 + 8.741942 + 17.331115 + 21.240802) / 4,  # 1 thread
    (4.697866 + 8.998151 + 17.979336 + 22.432767) / 4,  # 2 threads
    (4.308725 + 9.477705 + 22.935199 + 27.198649) / 4,  # 4 threads
    (4.951955 + 14.036810 + 32.365612 + 32.454227) / 4  # 8 threads
]

# A2 Implementation
a2_latency = [
    (1.621000 + 2.915320 + 3.383591 + 6.365880) / 4,
    (1.761020 + 3.385851 + 3.725410 + 7.066741) / 4,
    (2.118988 + 3.809943 + 4.562358 + 8.707209) / 4,
    (2.245692 + 5.972158 + 5.318249 + 10.614422) / 4
]

# A3 Implementation
a3_latency = [
    (2.461477 + 3.922544 + 4.802557 + 8.717568) / 4,
    (2.839393 + 4.285119 + 5.213585 + 9.518531) / 4,
    (3.344804 + 5.125460 + 6.321875 + 11.389109) / 4,
    (4.710199 + 6.234314 + 7.304777 + 17.548415) / 4
]

plt.figure()
plt.plot(thread_counts, a1_latency, marker='o', label='A1 (Two-Copy)', linewidth=2)
plt.plot(thread_counts, a2_latency, marker='s', label='A2 (One-Copy)', linewidth=2)
plt.plot(thread_counts, a3_latency, marker='^', label='A3 (Zero-Copy)', linewidth=2)

plt.xlabel('Number of Threads', fontsize=12)
plt.ylabel('Latency (μs)', fontsize=12)
plt.title('Latency vs Thread Count', fontsize=14, fontweight='bold')
plt.xticks(thread_counts)
plt.grid(True, alpha=0.3)
plt.legend(fontsize=10)
plt.tight_layout()
plt.savefig('MT25020_Part_D_Latency_vs_ThreadCount.png', dpi=300)
print("Saved: MT25020_Part_D_Latency_vs_ThreadCount.png")

# ==========================================
# PART C: Cache Misses vs Message Size
# ==========================================
print("Generating Plot 3: Cache Misses vs Message Size...")

# Hardcoded data from CSV - LLC Misses vs Message Size (averaged across threads)
# A1 Implementation
a1_llc = [
    (8775 + 3180 + 7343 + 48879) / 4,  # 1024 bytes
    (6547 + 7578 + 9900 + 21171) / 4,  # 4096 bytes
    (6055 + 15531 + 42580 + 93064) / 4,  # 16384 bytes
    (13404 + 11967 + 21787 + 46040) / 4  # 65536 bytes
]

# A2 Implementation
a2_llc = [
    (4751 + 3388 + 8776 + 78746) / 4,
    (1956 + 5353 + 11100 + 106316) / 4,
    (8234 + 8088 + 14439 + 764668) / 4,
    (4625 + 15820 + 28453 + 2520849) / 4
]

# A3 Implementation
a3_llc = [
    (8210 + 39965 + 53415 + 147335) / 4,
    (2476 + 3467 + 7166 + 120453) / 4,
    (5427 + 5587 + 10808 + 67251) / 4,
    (5572 + 12613 + 32452 + 360997) / 4
]

# L1 Cache Misses
# A1 Implementation
a1_l1 = [
    (75670871 + 175004381 + 327834123 + 614598598) / 4,
    (143090304 + 283963207 + 526405590 + 737453021) / 4,
    (224568817 + 411488100 + 663350646 + 1173606249) / 4,
    (511400337 + 955823034 + 1554967920 + 2845867127) / 4
]

# A2 Implementation
a2_l1 = [
    (136036463 + 249136894 + 418249976 + 701492742) / 4,
    (184748306 + 339560585 + 584316360 + 1054838042) / 4,
    (702175603 + 1278505613 + 2012412754 + 3597607737) / 4,
    (1232874698 + 2077273060 + 3387189605 + 6090317208) / 4
]

# A3 Implementation
a3_l1 = [
    (131925688 + 256866654 + 411337724 + 719704360) / 4,
    (190710723 + 356319588 + 602395615 + 932720042) / 4,
    (599527934 + 1109899007 + 1826112322 + 3256368607) / 4,
    (959535661 + 1774858641 + 2911453937 + 5026062699) / 4
]

# Create subplots
fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(15, 6))

# Plot LLC Misses
ax1.plot(message_sizes, a1_llc, marker='o', label='A1 (Two-Copy)', linewidth=2)
ax1.plot(message_sizes, a2_llc, marker='s', label='A2 (One-Copy)', linewidth=2)
ax1.plot(message_sizes, a3_llc, marker='^', label='A3 (Zero-Copy)', linewidth=2)
ax1.set_xlabel('Message Size (bytes)', fontsize=12)
ax1.set_ylabel('LLC Misses', fontsize=12)
ax1.set_title('LLC Cache Misses vs Message Size', fontsize=14, fontweight='bold')
ax1.set_xscale('log', base=2)
ax1.grid(True, alpha=0.3)
ax1.legend(fontsize=10)

# Plot L1 Misses
ax2.plot(message_sizes, a1_l1, marker='o', label='A1 (Two-Copy)', linewidth=2)
ax2.plot(message_sizes, a2_l1, marker='s', label='A2 (One-Copy)', linewidth=2)
ax2.plot(message_sizes, a3_l1, marker='^', label='A3 (Zero-Copy)', linewidth=2)
ax2.set_xlabel('Message Size (bytes)', fontsize=12)
ax2.set_ylabel('L1 Cache Misses', fontsize=12)
ax2.set_title('L1 Cache Misses vs Message Size', fontsize=14, fontweight='bold')
ax2.set_xscale('log', base=2)
ax2.grid(True, alpha=0.3)
ax2.legend(fontsize=10)

plt.tight_layout()
plt.savefig('MT25020_Part_D_CacheMisses_vs_MessageSize.png', dpi=300)
print("Saved: MT25020_Part_D_CacheMisses_vs_MessageSize.png")

# ==========================================
# PART D: CPU Cycles per Byte
# ==========================================
print("Generating Plot 4: CPU Cycles per Byte...")

# Hardcoded data from CSV - CPU Cycles per Byte
# A1 Implementation
a1_cycles_per_byte = [
    ((20358750776/(1.813721*1e9/8*10)) + (48319424928/(1.743771*1e9/8*10)) + 
     (97339949176/(1.901260*1e9/8*10)) + (191381196108/(1.654298*1e9/8*10))) / 4,  # 1024
    ((24329939989/(3.748368*1e9/8*10)) + (48793263189/(3.641640*1e9/8*10)) + 
     (95473420086/(3.457383*1e9/8*10)) + (139569468254/(2.334435*1e9/8*10))) / 4,  # 4096
    ((24015267201/(7.562831*1e9/8*10)) + (46850464681/(7.290157*1e9/8*10)) + 
     (75905887486/(5.714884*1e9/8*10)) + (135747038645/(4.049747*1e9/8*10))) / 4,  # 16384
    ((23957001222/(24.683154*1e9/8*10)) + (44930054814/(23.371526*1e9/8*10)) + 
     (76368539974/(19.276332*1e9/8*10)) + (134423028524/(16.154771*1e9/8*10))) / 4  # 65536
]

# A2 Implementation
a2_cycles_per_byte = [
    ((24224260670/(5.053669*1e9/8*10)) + (44993453054/(4.651849*1e9/8*10)) + 
     (77271359447/(3.865997*1e9/8*10)) + (136910667173/(3.647873*1e9/8*10))) / 4,
    ((24151783958/(11.239933*1e9/8*10)) + (44124908616/(9.677922*1e9/8*10)) + 
     (75539964918/(8.600654*1e9/8*10)) + (128873062257/(5.486799*1e9/8*10))) / 4,
    ((23781723827/(38.737563*1e9/8*10)) + (43902500225/(35.183268*1e9/8*10)) + 
     (73962976272/(28.729002*1e9/8*10)) + (130252427652/(24.645704*1e9/8*10))) / 4,
    ((23966912654/(82.359073*1e9/8*10)) + (44334107806/(74.190916*1e9/8*10)) + 
     (74708748286/(60.213093*1e9/8*10)) + (130538193114/(49.393928*1e9/8*10))) / 4
]

# A3 Implementation
a3_cycles_per_byte = [
    ((24024035563/(3.328083*1e9/8*10)) + (44057901662/(2.885124*1e9/8*10)) + 
     (74968731759/(2.449172*1e9/8*10)) + (133752541390/(1.739205*1e9/8*10))) / 4,
    ((24088300044/(8.353762*1e9/8*10)) + (44164421961/(7.646930*1e9/8*10)) + 
     (75483646679/(6.393183*1e9/8*10)) + (134376124316/(5.256072*1e9/8*10))) / 4,
    ((24323357899/(27.292126*1e9/8*10)) + (45275373152/(25.140473*1e9/8*10)) + 
     (77224511985/(20.733090*1e9/8*10)) + (135930055621/(17.943326*1e9/8*10))) / 4,
    ((23930500637/(60.141648*1e9/8*10)) + (45024829885/(55.080877*1e9/8*10)) + 
     (76537571571/(46.034261*1e9/8*10)) + (133864228753/(29.876765*1e9/8*10))) / 4
]

plt.figure()
plt.plot(message_sizes, a1_cycles_per_byte, marker='o', label='A1 (Two-Copy)', linewidth=2)
plt.plot(message_sizes, a2_cycles_per_byte, marker='s', label='A2 (One-Copy)', linewidth=2)
plt.plot(message_sizes, a3_cycles_per_byte, marker='^', label='A3 (Zero-Copy)', linewidth=2)

plt.xlabel('Message Size (bytes)', fontsize=12)
plt.ylabel('CPU Cycles per Byte', fontsize=12)
plt.title('CPU Cycles per Byte Transferred', fontsize=14, fontweight='bold')
plt.xscale('log', base=2)
plt.grid(True, alpha=0.3)
plt.legend(fontsize=10)
plt.tight_layout()
plt.savefig('MT25020_Part_D_CPUCycles_per_Byte.png', dpi=300)
print("Saved: MT25020_Part_D_CPUCycles_per_Byte.png")