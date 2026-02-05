# MT25020
import matplotlib.pyplot as plt
import numpy as np

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

plt.figure(figsize=(10, 6))
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
print("Plot saved: MT25020_Part_D_Throughput_vs_MessageSize.png")
plt.show()