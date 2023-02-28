CC = gcc
CFLAGS = -Wall -pedantic -std=c99
SRCDIR = src
INCDIR = include
OBJDIR = obj
BINDIR = bin

TARGET = $(BINDIR)/program

SRC = $(wildcard $(SRCDIR)/*.c)
OBJ = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRC))

INC = -I$(INCDIR)

all: create_directories $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(INC) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(INCDIR)/*.h
	$(CC) -g $(CFLAGS) $(INC) -c $< -o $@

run: all
	./$(TARGET)

debug: all
	gdb $(TARGET) --tui

memorycheck: all
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TARGET)

clean:
	rm -f $(OBJDIR)/*.o $(TARGET)
	rmdir $(OBJDIR)
	rmdir $(BINDIR)

create_directories:
	mkdir -p $(OBJDIR)
	mkdir -p $(BINDIR)