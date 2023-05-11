CC = gcc
CFLAGS = -Wall -pedantic -std=c99

COLLECTORDIR = collector
MASTERWORKERDIR = masterworker

SRCDIR_MW = $(MASTERWORKERDIR)/src
INCDIR_MW = $(MASTERWORKERDIR)/include
OBJDIR_MW = $(MASTERWORKERDIR)/obj

SRC_MW = $(wildcard $(SRCDIR_MW)/*.c)
OBJ_MW = $(patsubst $(SRCDIR_MW)/%.c, $(OBJDIR_MW)/%.o, $(SRC_MW))

INC_MW = -I$(INCDIR_MW)

SRCDIR_C = $(COLLECTORDIR)/src
INCDIR_C = $(COLLECTORDIR)/include
OBJDIR_C = $(COLLECTORDIR)/obj

SRC_C = $(wildcard $(SRCDIR_C)/*.c)
OBJ_C = $(patsubst $(SRCDIR_C)/%.c, $(OBJDIR_C)/%.o, $(SRC_C))

INC_C = -I$(INCDIR_C)

SRC_GENERAFILE = generafile.c

TARGET_FARM = farm

TARGET_GENERAFILE = generafile

all: create_directories $(TARGET_FARM)

$(TARGET_FARM): $(OBJ_MW) $(OBJ_C)
	$(CC) $(CFLAGS) $(INC_MW) $(INC_C) -o $@ $^

$(OBJDIR_MW)/%.o: $(SRCDIR_MW)/%.c $(INCDIR_MW)/*.h
	$(CC) -g $(CFLAGS) $(INC_MW) -c $< -o $@

$(OBJDIR_C)/%.o: $(SRCDIR_C)/%.c $(INCDIR_C)/*.h
	$(CC) -g $(CFLAGS) $(INC_C) -c $< -o $@

run: all
	./$(TARGET_FARM) -d testdir

debug: all
	gdb $(TARGET_FARM) --tui

memorycheck: all
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TARGET_FARM) -d testdir

generafile: generafile.o
	$(CC) $(CFLAGS) -o $@ $^ 

generafile.o: generafile.c
	$(CC) -g $(CFLAGS) -c $< -o $@

test: generafile all
	./test.sh

clean:
	rm -f $(OBJDIR_MW)/*.o $(TARGET_FARM)
	rm -rf $(OBJDIR_MW)
	rm -f $(OBJDIR_C)/*.o
	rm -rf $(OBJDIR_C)
	rm -f ./*.dat
	rm -f ./*.o
	rm -f ./*.txt

create_directories:
	mkdir -p $(OBJDIR_MW)
	mkdir -p $(OBJDIR_C)
