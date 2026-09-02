CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = file_manager
SOURCES = main.c file_operations.c directory_operations.c search_operation.c file_manager.c
OBJECTS = $(SOURCES:.c=.o)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(CFLAGS)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -f $(TARGET) $(OBJECTS)

run: $(TARGET)
	./$(TARGET)