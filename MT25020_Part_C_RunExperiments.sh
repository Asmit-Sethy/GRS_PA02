#!/bin/bash
# MT25020

# Configuration
SERVER_IP="127.0.0.1"
PORT=8080
MESSAGE_SIZES=(1024 4096 16384 65536)
THREAD_COUNTS=(1 2 4 8)
DURATION=10
OUTPUT_CSV="MT25020_Part_C_Results.csv"

# Compile all implementations
echo "Compiling implementations..."
make clean
make all

if [ $? -ne 0 ]; then
    echo "Compilation failed!"
    exit 1
fi

# Initialize CSV file with header
echo "Impl,MsgSize,Threads,ThroughputGbps,LatencyUs,CPUCycles,L1Misses,LLCMisses,ContextSwitches" > $OUTPUT_CSV

# Function to run experiment
run_experiment() {
    local impl=$1
    local msg_size=$2
    local num_threads=$3
    
    echo "Running $impl with message_size=$msg_size, threads=$num_threads"
    
    # Start server in background
    if [ "$impl" == "A1" ]; then
        ./MT25020_Part_A1_Server $msg_size $PORT &
    elif [ "$impl" == "A2" ]; then
        ./MT25020_Part_A2_Server $msg_size $PORT &
    else
        ./MT25020_Part_A3_Server $msg_size $PORT &
    fi
    
    SERVER_PID=$!
    sleep 2  # Wait for server to start
    
    # Run client with perf
    if [ "$impl" == "A1" ]; then
        CLIENT_OUTPUT=$(perf stat -e cycles,L1-dcache-load-misses,LLC-load-misses,context-switches \
            ./MT25020_Part_A1_Client $SERVER_IP $PORT $msg_size $num_threads 2>&1)
    elif [ "$impl" == "A2" ]; then
        CLIENT_OUTPUT=$(perf stat -e cycles,L1-dcache-load-misses,LLC-load-misses,context-switches \
            ./MT25020_Part_A2_Client $SERVER_IP $PORT $msg_size $num_threads 2>&1)
    else
        CLIENT_OUTPUT=$(perf stat -e cycles,L1-dcache-load-misses,LLC-load-misses,context-switches \
            ./MT25020_Part_A3_Client $SERVER_IP $PORT $msg_size $num_threads 2>&1)
    fi
    
    # Kill server
    kill $SERVER_PID 2>/dev/null
    wait $SERVER_PID 2>/dev/null
    
    # Parse client output
    THROUGHPUT=$(echo "$CLIENT_OUTPUT" | grep "Throughput:" | awk '{print $2}')
    LATENCY=$(echo "$CLIENT_OUTPUT" | grep "Latency:" | awk '{print $2}')
    
    # Parse perf output
    CPU_CYCLES=$(echo "$CLIENT_OUTPUT" | grep "cycles" | awk '{gsub(/,/, ""); print $1}')
    L1_MISSES=$(echo "$CLIENT_OUTPUT" | grep "L1-dcache-load-misses" | awk '{gsub(/,/, ""); print $1}')
    LLC_MISSES=$(echo "$CLIENT_OUTPUT" | grep "LLC-load-misses" | awk '{gsub(/,/, ""); print $1}')
    CTX_SWITCHES=$(echo "$CLIENT_OUTPUT" | grep "context-switches" | awk '{gsub(/,/, ""); print $1}')
    
    # Default values if parsing failed
    [ -z "$THROUGHPUT" ] && THROUGHPUT="0.0"
    [ -z "$LATENCY" ] && LATENCY="0.0"
    [ -z "$CPU_CYCLES" ] && CPU_CYCLES="0"
    [ -z "$L1_MISSES" ] && L1_MISSES="0"
    [ -z "$LLC_MISSES" ] && LLC_MISSES="0"
    [ -z "$CTX_SWITCHES" ] && CTX_SWITCHES="0"
    
    # Append to CSV
    echo "$impl,$msg_size,$num_threads,$THROUGHPUT,$LATENCY,$CPU_CYCLES,$L1_MISSES,$LLC_MISSES,$CTX_SWITCHES" >> $OUTPUT_CSV
    
    # Small delay between experiments
    sleep 1
}

# Run all experiments
for impl in "A1" "A2" "A3"; do
    for msg_size in "${MESSAGE_SIZES[@]}"; do
        for num_threads in "${THREAD_COUNTS[@]}"; do
            run_experiment $impl $msg_size $num_threads
        done
    done
done

echo "All experiments completed!"
echo "Results saved to $OUTPUT_CSV"