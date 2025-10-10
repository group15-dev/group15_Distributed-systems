CC = gcc
CFLAGS = -Wall -g -std=c99

# Server executable
SERVER_TARGET = src/server/chat
SERVER_SOURCES = src/server/chat.c
SERVER_OBJECTS = $(SERVER_SOURCES:.c=.o)

# Client executable  
CLIENT_TARGET = src/client/chat_client
CLIENT_SOURCES = src/client/main.c src/client/client.c
CLIENT_OBJECTS = $(CLIENT_SOURCES:.c=.o)

# Legacy server (old implementation)
LEGACY_SERVER_TARGET = src/server/server
LEGACY_SERVER_SOURCES = src/server/main.c src/server/server.c
LEGACY_SERVER_OBJECTS = $(LEGACY_SERVER_SOURCES:.c=.o)

all: $(SERVER_TARGET) $(CLIENT_TARGET) $(LEGACY_SERVER_TARGET)

# Build the new distributed chat server
$(SERVER_TARGET): $(SERVER_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

# Build the chat client
$(CLIENT_TARGET): $(CLIENT_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

# Build the legacy server
$(LEGACY_SERVER_TARGET): $(LEGACY_SERVER_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

# Compile object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(SERVER_OBJECTS) $(CLIENT_OBJECTS) $(LEGACY_SERVER_OBJECTS)
	rm -f $(SERVER_TARGET) $(CLIENT_TARGET) $(LEGACY_SERVER_TARGET)

# Run server node A (port 9001, peer to 9002)
run-server-a: $(SERVER_TARGET)
	./$(SERVER_TARGET) 9001 127.0.0.1 9002 NodeA

# Run server node B (port 9002, peer to 9001)  
run-server-b: $(SERVER_TARGET)
	./$(SERVER_TARGET) 9002 127.0.0.1 9001 NodeB

# Run client example
run-client: $(CLIENT_TARGET)
	@echo "Usage: make run-client USER=alice ROOM=general PORT=9001"
	@echo "Example: make run-client USER=alice ROOM=general PORT=9001"

.PHONY: all clean run-server-a run-server-b run-client
