#!/bin/bash
# MT25020

# Configuration
SERVER_IP="10.0.0.1" 
PORT=8080
MESSAGE_SIZES=(1024 4096 16384 65536)
THREAD_COUNTS=(1 2 4 8)
OUTPUT_CSV="MT25020_Part_C_Results.csv"

# 1. Setup Network Namespaces
echo "Setting up network namespaces..."
# Clean up old namespaces if they exist
sudo ip netns del ns_server 2>/dev/null
sudo ip netns del ns_client 2>/dev/null

# Create new namespaces
sudo ip netns add ns_server
sudo ip netns add ns_client

# Create veth pair
sudo ip link add veth-server type veth peer name veth-client

# Plug ends into namespaces
sudo ip link set veth-server netns ns_server
sudo ip link set veth-client netns ns_client

# Assign IPs and bring interfaces UP
sudo ip netns exec ns_server ip addr add 10.0.0.1/24 dev veth-server
sudo ip netns exec ns_server ip link set veth-server up
sudo ip netns exec ns_server ip link set lo up

sudo ip netns exec ns_client ip addr add 10.0.0.2/24 dev veth-client
sudo ip netns exec ns_client ip link set veth-client up
sudo ip netns exec ns_client ip link set lo up

# Enable forwarding
sudo sysctl -w net.ipv4.ip_forward=1 > /dev/null

# 2. Compile Code
echo "Compiling..."
make clean
make all

# CSV Header (Restored LLCMisses as per assignment)
echo "Impl,MsgSize,Threads,ThroughputGbps,LatencyUs,CPUCycles,L1Misses,LLCMisses,ContextSwitches" > $OUTPUT_CSV

run_experiment() {
    local impl=$1
    local msg_size=$2
    local num_threads=$3
    
    echo "Running $impl: Size=$msg_size, Threads=$num_threads"
    
    # Kill zombies globally and inside namespace
    sudo ip netns exec ns_server killall -9 MT25020_Part_A1_Server MT25020_Part_A2_Server MT25020_Part_A3_Server 2>/dev/null
    sudo killall -9 MT25020_Part_A1_Server MT25020_Part_A2_Server MT25020_Part_A3_Server 2>/dev/null
    sleep 1

    # 3. Start Server INSIDE 'ns_server' namespace
    sudo ip netns exec ns_server ./MT25020_Part_${impl}_Server $msg_size $PORT &
    SERVER_PID=$!
    
    sleep 2  # Wait for server to bind

    # 4. Run Client INSIDE 'ns_client' namespace
    # Using 'LLC-load-misses' as requested
    CLIENT_OUTPUT=$(sudo ip netns exec ns_client perf stat -e cycles,L1-dcache-load-misses,LLC-load-misses,context-switches \
        ./MT25020_Part_${impl}_Client $SERVER_IP $PORT $msg_size $num_threads 2>&1)
    
    # 5. Cleanup
    sudo kill -9 $SERVER_PID 2>/dev/null
    wait $SERVER_PID 2>/dev/null
    
    # 6. Parsing
    THROUGHPUT=$(echo "$CLIENT_OUTPUT" | grep "Throughput:" | awk '{print $2}')
    LATENCY=$(echo "$CLIENT_OUTPUT" | grep "Latency:" | awk '{print $2}')
    
    # Robust perf parsing
    CPUCycles=$(echo "$CLIENT_OUTPUT" | grep "cycles" | sed 's/,//g' | awk '{print $1}')
    L1Misses=$(echo "$CLIENT_OUTPUT" | grep "L1-dcache-load-misses" | sed 's/,//g' | awk '{print $1}')
    LLCMisses=$(echo "$CLIENT_OUTPUT" | grep "LLC-load-misses" | sed 's/,//g' | awk '{print $1}')
    ContextSwitches=$(echo "$CLIENT_OUTPUT" | grep "context-switches" | sed 's/,//g' | awk '{print $1}')
    
    # Defaults (Defaults to 0 if <not supported> occurs)
    THROUGHPUT=${THROUGHPUT:-0}
    LATENCY=${LATENCY:-0}
    CPUCycles=${CPUCycles:-0}
    L1Misses=${L1Misses:-0}
    LLCMisses=${LLCMisses:-0}
    ContextSwitches=${ContextSwitches:-0}

    echo "$impl,$msg_size,$num_threads,$THROUGHPUT,$LATENCY,$CPUCycles,$L1Misses,$LLCMisses,$ContextSwitches" >> $OUTPUT_CSV
}

# Run Loop
for impl in "A1" "A2" "A3"; do
    for msg_size in "${MESSAGE_SIZES[@]}"; do
        for num_threads in "${THREAD_COUNTS[@]}"; do
            run_experiment $impl $msg_size $num_threads
        done
    done
done

# Cleanup Namespaces
echo "Cleaning up namespaces..."
sudo ip netns del ns_server
sudo ip netns del ns_client

echo "Done. Results in $OUTPUT_CSV"