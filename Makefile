CC=gcc
TARGET=console-menus
SOURCES=main.c console.c
CFLAGS=-O3
LFLAGS=-lm

################################################################################

# define list of objects
OBJSC=$(SOURCES:.c=.o)
OBJS=$(OBJSC:.cpp=.o)

# the target is obtained linking all .o files
all: $(SOURCES) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o $(TARGET)

purge: clean
	rm -f $(TARGET)

clean:
	rm -f *.o

################################################################################