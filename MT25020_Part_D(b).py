# MT25020
import matplotlib.pyplot as plt
import numpy as np

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

plt.figure(figsize=(10, 6))
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
print("Plot saved: MT25020_Part_D_Latency_vs_ThreadCount.png")
plt.show()