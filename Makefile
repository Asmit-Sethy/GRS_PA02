# MT25020
CC = gcc
CFLAGS = -Wall -pthread -O2
TARGETS = MT25020_Part_A1_Server MT25020_Part_A1_Client \
          MT25020_Part_A2_Server MT25020_Part_A2_Client \
          MT25020_Part_A3_Server MT25020_Part_A3_Client

all: $(TARGETS)

MT25020_Part_A1_Server: MT25020_Part_A1_Server.c MT25020_Common.h
	$(CC) $(CFLAGS) MT25020_Part_A1_Server.c -o MT25020_Part_A1_Server

MT25020_Part_A1_Client: MT25020_Part_A1_Client.c MT25020_Common.h
	$(CC) $(CFLAGS) MT25020_Part_A1_Client.c -o MT25020_Part_A1_Client

MT25020_Part_A2_Server: MT25020_Part_A2_Server.c MT25020_Common.h
	$(CC) $(CFLAGS) MT25020_Part_A2_Server.c -o MT25020_Part_A2_Server

MT25020_Part_A2_Client: MT25020_Part_A2_Client.c MT25020_Common.h
	$(CC) $(CFLAGS) MT25020_Part_A2_Client.c -o MT25020_Part_A2_Client

MT25020_Part_A3_Server: MT25020_Part_A3_Server.c MT25020_Common.h
	$(CC) $(CFLAGS) MT25020_Part_A3_Server.c -o MT25020_Part_A3_Server

MT25020_Part_A3_Client: MT25020_Part_A3_Client.c MT25020_Common.h
	$(CC) $(CFLAGS) MT25020_Part_A3_Client.c -o MT25020_Part_A3_Client

clean:
	rm -f $(TARGETS) *.o

.PHONY: all clean