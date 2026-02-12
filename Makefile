CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iinclude
OBJDIR = bin

SERVER_SRC = src/tcp_server.c src/signal.c src/SIGCHLD_handler.c src/str_add.c src/str_cli.c
SERVER_OBJ = $(SERVER_SRC:src/%.c=$(OBJDIR)/%.o)
SERVER_TARGET = $(OBJDIR)/tcp_server

CLIENT_SRC = src/client.c
CLIENT_TARGET = $(OBJDIR)/client

all: $(SERVER_TARGET) $(CLIENT_TARGET)

$(SERVER_TARGET): $(SERVER_OBJ)
	$(CC) $(CFLAGS) $(SERVER_OBJ) -o $(SERVER_TARGET)

$(CLIENT_TARGET): $(CLIENT_SRC)
	$(CC) $(CFLAGS) $(CLIENT_SRC) -o $(CLIENT_TARGET)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(SERVER_OBJ): | $(OBJDIR)

$(OBJDIR)/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR)

.PHONY: all clean
