#!/bin/bash

# Distributed Chat System Runner
# This script helps you run the distributed chat system

echo "=== Distributed Chat System ==="
echo ""

# Check if make is available
if ! command -v make &> /dev/null; then
    echo "Error: make is not installed. Please install make to build the project."
    exit 1
fi

# Build the project
echo "Building the chat system..."
make clean
make all

if [ $? -ne 0 ]; then
    echo "Build failed. Please check the error messages above."
    exit 1
fi

echo "Build successful!"
echo ""

# Show usage instructions
echo "=== Usage Instructions ==="
echo ""
echo "1. Start Server Node A (port 9001):"
echo "   make run-server-a"
echo ""
echo "2. Start Server Node B (port 9002):"
echo "   make run-server-b"
echo ""
echo "3. Start clients in separate terminals:"
echo "   ./src/client/chat_client 9001 alice general"
echo "   ./src/client/chat_client 9002 bob general"
echo "   ./src/client/chat_client 9001 charlie general"
echo ""
echo "4. Test distributed chat:"
echo "   - Alice and Charlie connect to Node A"
echo "   - Bob connects to Node B"
echo "   - All three can chat in the 'general' room"
echo "   - Messages are relayed between nodes automatically"
echo ""
echo "=== Quick Start ==="
echo "Run these commands in separate terminals:"
echo ""
echo "Terminal 1: make run-server-a"
echo "Terminal 2: make run-server-b"  
echo "Terminal 3: ./src/client/chat_client 9001 alice general"
echo "Terminal 4: ./src/client/chat_client 9002 bob general"
echo ""
echo "Then type messages in the client terminals to test the distributed chat!"