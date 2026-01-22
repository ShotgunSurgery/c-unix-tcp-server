# deifine variables
# general syntax for variable definition - NAME = value
# general syntax for variable use - $(NAME)
# define compiler first
cc = gcc
# define flags second
CFLAGS = -Wall -Wextra -std=c11 -Iinclude
# define a dir for objs
OBJDIR = bin
# define which files be compiled
SRC = src/tcp_echo_server.c src/signal.c src/SIGCHLD_handler.c
# for each .c make a .o   
OBJ = $(SRC:src/%.c=$(OBJDIR)/%.o)
# set a final target main executable
TARGET = $(OBJDIR)/tcp_echo_server

# define rules
# general syntax for definition - 
# target: dependencies
# <TAB>command
# now make will run the entire file and stuff like make clean will only run the defined rule

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

# create bin if it doesn't exists 
$(OBJDIR): 
	mkdir -p $(OBJDIR)

# ensure bin exists befoure compilation
$(OBJ): | $(OBJDIR)

all : $(TARGET)

# define a clean rule
clean : 
	rm -rf $(OBJDIR)

client :
	g++ src/tcp_echo_client.c -o bin/tcp_echo_client

# what is this ? 
.PHONY: all clean

# how to compile each source file into object files
$(OBJDIR)/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@
