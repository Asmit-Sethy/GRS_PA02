#!/bin/bash
# MT25020

# Configuration
SERVER_IP="10.0.0.1" 
PORT=8080
MESSAGE_SIZES=(1024 4096 16384 65536)
THREAD_COUNTS=(1 2 4 8)
OUTPUT_CSV="MT25020_Part_C_Results.csv"

# --- 0. FORCE PERF PERMISSIONS ---
# Try to unblock hardware counters automatically
sysctl -w kernel.perf_event_paranoid=-1 2>/dev/null

# --- NETWORK NAMESPACE SETUP ---
setup_namespaces() {
    echo "Setting up network namespaces..."
    ip netns del ns_server 2>/dev/null
    ip netns del ns_client 2>/dev/null

    ip netns add ns_server
    ip netns add ns_client

    ip link add veth_server type veth peer name veth_client

    ip link set veth_server netns ns_server
    ip link set veth_client netns ns_client

    ip netns exec ns_server ip addr add 10.0.0.1/24 dev veth_server
    ip netns exec ns_server ip link set veth_server up
    ip netns exec ns_server ip link set lo up

    ip netns exec ns_client ip addr add 10.0.0.2/24 dev veth_client
    ip netns exec ns_client ip link set veth_client up
    ip netns exec ns_client ip link set lo up
    
    # Enable offloading
    ip netns exec ns_client ethtool -K veth_client sg on tso on 2>/dev/null
    ip netns exec ns_server ethtool -K veth_server sg on tso on 2>/dev/null
}

cleanup_namespaces() {
    echo "Cleaning up namespaces..."
    ip netns del ns_server 2>/dev/null
    ip netns del ns_client 2>/dev/null
}

# --- COMPILE ---
echo "Compiling implementations..."
make clean
make all

setup_namespaces

# Initialize CSV with correct hardware headers
echo "Impl,MsgSize,Threads,ThroughputGbps,LatencyUs,CPUCycles,L1Misses,LLCMisses,ContextSwitches" > $OUTPUT_CSV

# Function to run experiment
run_experiment() {
    local impl=$1
    local msg_size=$2
    local num_threads=$3
    
    echo "Running $impl with message_size=$msg_size, threads=$num_threads"
    
    # 1. Start Server pinned to Core 2 (P-Core) to separate it from client
    ip netns exec ns_server taskset -c 2 ./MT25020_Part_${impl}_Server $msg_size $PORT &
    SERVER_PID=$!
    
    sleep 2
    
    # 2. Run Client pinned to Core 0 (P-Core)
    # We ask for REAL hardware counters using the 'cpu_core' prefix for hybrid CPUs.
    CLIENT_OUTPUT=$(ip netns exec ns_client taskset -c 0 perf stat \
        -e cpu_core/cycles/,cpu_core/L1-dcache-load-misses/,cpu_core/LLC-load-misses/,context-switches \
        ./MT25020_Part_${impl}_Client $SERVER_IP $PORT $msg_size $num_threads 2>&1)
    
    # 3. Kill server
    kill -9 $SERVER_PID 2>/dev/null
    wait $SERVER_PID 2>/dev/null
    
    # 4. Parsing Logic
    THROUGHPUT=$(echo "$CLIENT_OUTPUT" | grep "Throughput:" | awk '{print $2}')
    LATENCY=$(echo "$CLIENT_OUTPUT" | grep "Latency:" | awk '{print $2}')
    
    # PERF PARSING
    # 1. cpu_core/cycles/
    CPU_CYCLES=$(echo "$CLIENT_OUTPUT" | grep "cpu_core/cycles/" | head -n 1 | sed 's/,//g' | awk '{print $1}')
    
    # 2. cpu_core/L1-dcache-load-misses/
    L1_MISSES=$(echo "$CLIENT_OUTPUT" | grep "cpu_core/L1-dcache-load-misses/" | head -n 1 | sed 's/,//g' | awk '{print $1}')
    
    # 3. cpu_core/LLC-load-misses/
    LLC_MISSES=$(echo "$CLIENT_OUTPUT" | grep "cpu_core/LLC-load-misses/" | head -n 1 | sed 's/,//g' | awk '{print $1}')
    
    # 4. Context Switches
    CTX_SWITCHES=$(echo "$CLIENT_OUTPUT" | grep "context-switches" | head -n 1 | sed 's/,//g' | awk '{print $1}')
    
    # 5. Sanitize Zeros
    if [[ "$CPU_CYCLES" == *"<"* ]] || [ -z "$CPU_CYCLES" ]; then CPU_CYCLES="0"; fi
    if [[ "$L1_MISSES" == *"<"* ]] || [ -z "$L1_MISSES" ]; then L1_MISSES="0"; fi
    if [[ "$LLC_MISSES" == *"<"* ]] || [ -z "$LLC_MISSES" ]; then LLC_MISSES="0"; fi
    if [[ "$CTX_SWITCHES" == *"<"* ]] || [ -z "$CTX_SWITCHES" ]; then CTX_SWITCHES="0"; fi

    THROUGHPUT=${THROUGHPUT:-0.0}
    LATENCY=${LATENCY:-0.0}
    
    echo "$impl,$msg_size,$num_threads,$THROUGHPUT,$LATENCY,$CPU_CYCLES,$L1_MISSES,$LLC_MISSES,$CTX_SWITCHES" >> $OUTPUT_CSV
}

# Run all experiments
for impl in "A1" "A2" "A3"; do
    for msg_size in "${MESSAGE_SIZES[@]}"; do
        for num_threads in "${THREAD_COUNTS[@]}"; do
            run_experiment $impl $msg_size $num_threads
        done
    done
done

cleanup_namespaces
echo "Results saved to $OUTPUT_CSV"