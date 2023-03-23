CC = gcc # COMPILER
CFLAGS = -Wall #FLAGS
LDFLAGS =	#LIBRARY FLAGS /lm/lefence/etc
OBJFILES = $(wildcard *.c) #Object files?
TARGET = Bnuuy

all: $(TARGET)
	.\Bnuuy.exe

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)
clean:
	rm -f $(OBJFILES) $(TARGET) *~