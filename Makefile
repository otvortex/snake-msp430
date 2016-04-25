CC = clang
  # compiler flags:
  #  -g    adds debugging information to the executable file
  #  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -std=c99 -Werror
  # the build target executable:

TARGET = main

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

all: $(TARGET)

clean:
	rm -rf $(TARGET)