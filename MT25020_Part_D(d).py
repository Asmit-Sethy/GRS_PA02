# MT25020
import matplotlib.pyplot as plt
import numpy as np

# Hardcoded data from CSV - CPU Cycles per Byte
message_sizes = [1024, 4096, 16384, 65536]

# For calculating cycles per byte, we need to estimate bytes transferred
# Assuming 10 seconds runtime and throughput values
# Bytes = (Throughput_Gbps * 1e9 / 8) * 10 seconds

# A1 Implementation - CPU cycles per byte (averaged)
# Formula: CPU_Cycles / (estimated_bytes_transferred)
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

plt.figure(figsize=(10, 6))
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
print("Plot saved: MT25020_Part_D_CPUCycles_per_Byte.png")
plt.show()