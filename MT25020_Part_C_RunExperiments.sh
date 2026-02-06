#!/bin/bash
# MT25020

# Configuration
SERVER_IP="10.0.0.1" 
PORT=8080
MESSAGE_SIZES=(1024 4096 16384 65536)
THREAD_COUNTS=(1 2 4 8)
OUTPUT_CSV="MT25020_Part_C_Results.csv"
PLOT_SCRIPT="MT25020_Part_D_Plots.py"

# --- FORCE PERF PERMISSIONS ---
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
    
    ip netns exec ns_client ethtool -K veth_client sg on tso on 2>/dev/null
    ip netns exec ns_server ethtool -K veth_server sg on tso on 2>/dev/null
}

cleanup_namespaces() {
    ip netns del ns_server 2>/dev/null
    ip netns del ns_client 2>/dev/null
}

# --- COMPILE ---
echo "Compiling implementations..."
make clean
make all

setup_namespaces

# Initialize CSV
echo "Impl,MsgSize,Threads,ThroughputGbps,LatencyUs,CPUCycles,L1Misses,LLCMisses,ContextSwitches" > $OUTPUT_CSV

run_experiment() {
    local impl=$1
    local msg_size=$2
    local num_threads=$3
    
    echo "Running $impl with message_size=$msg_size, threads=$num_threads"
    
    # --- 1. START SERVER (Background, Pinned to Core 2) ---
    # We do NOT wrap with perf here. We just start the process.
    ip netns exec ns_server taskset -c 2 ./MT25020_Part_${impl}_Server $msg_size $PORT &
    SERVER_PID=$!
    
    # --- 2. WAIT FOR SERVER READY ---
    # Loop until 'ss' shows the port is listening inside the namespace
    # This prevents the client from connecting too early (Connection Refused errors)
    echo "Waiting for Server to be READY..."
    while ! ip netns exec ns_server ss -lnt | grep -q ":$PORT"; do
        sleep 0.1
    done
    
    # --- 3. START PERF ATTACHED TO SERVER PID ---
    # We run perf inside the namespace and attach to the specific PID (-p).
    # We output to a file (-o) and run it in background (&).
    ip netns exec ns_server perf stat \
        -p $SERVER_PID \
        -e cpu_core/cycles/,cpu_core/L1-dcache-load-misses/,cpu_core/LLC-load-misses/,context-switches \
        -o server_perf.log 2>&1 &
    
    PERF_PID=$!
    
    # Give perf a moment to attach
    sleep 0.5
    
    # --- 4. RUN CLIENT (Foreground) ---
    # Client (Receiver) runs on P-Core 0.
    CLIENT_OUTPUT=$(ip netns exec ns_client taskset -c 0 ./MT25020_Part_${impl}_Client $SERVER_IP $PORT $msg_size $num_threads 2>&1)
    
    # --- 5. STOP PERF & SERVER ---
    # Send SIGINT to Perf to ensure it flushes stats to the file
    kill -2 $PERF_PID 2>/dev/null
    wait $PERF_PID 2>/dev/null
    
    # Now kill the server
    kill -9 $SERVER_PID 2>/dev/null
    wait $SERVER_PID 2>/dev/null
    
    # --- 6. PARSE RESULTS ---
    
    # Parse App Metrics (From Client Output)
    THROUGHPUT=$(echo "$CLIENT_OUTPUT" | grep "Throughput:" | awk '{print $2}')
    LATENCY=$(echo "$CLIENT_OUTPUT" | grep "Latency:" | awk '{print $2}')
    
    # Parse Perf Metrics (From server_perf.log)
    CPU_CYCLES=$(grep "cpu_core/cycles/" server_perf.log | head -n 1 | sed 's/,//g' | awk '{print $1}')
    L1_MISSES=$(grep "cpu_core/L1-dcache-load-misses/" server_perf.log | head -n 1 | sed 's/,//g' | awk '{print $1}')
    LLC_MISSES=$(grep "cpu_core/LLC-load-misses/" server_perf.log | head -n 1 | sed 's/,//g' | awk '{print $1}')
    CTX_SWITCHES=$(grep "context-switches" server_perf.log | head -n 1 | sed 's/,//g' | awk '{print $1}')
    
    # Sanitize
    THROUGHPUT=${THROUGHPUT:-0.0}
    LATENCY=${LATENCY:-0.0}
    CPU_CYCLES=${CPU_CYCLES:-0}
    L1_MISSES=${L1_MISSES:-0}
    LLC_MISSES=${LLC_MISSES:-0}
    CTX_SWITCHES=${CTX_SWITCHES:-0}

    # Handle <not supported> by setting to 0
    if [[ "$CPU_CYCLES" == *"<"* ]]; then CPU_CYCLES="0"; fi
    if [[ "$L1_MISSES" == *"<"* ]]; then L1_MISSES="0"; fi
    if [[ "$LLC_MISSES" == *"<"* ]]; then LLC_MISSES="0"; fi
    
    echo "$impl,$msg_size,$num_threads,$THROUGHPUT,$LATENCY,$CPU_CYCLES,$L1_MISSES,$LLC_MISSES,$CTX_SWITCHES" >> $OUTPUT_CSV
    
    # Clean temp file
    rm -f server_perf.log
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

# Run plotting script
echo "Generating plots..."
if [ -f "$PLOT_SCRIPT" ]; then
    python3 "$PLOT_SCRIPT"
else
    echo "Plot script not found."
fi