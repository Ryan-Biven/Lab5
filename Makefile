CC = g++
CFLAGS = -std=c++17

matrix_operations: matrix_operations.cpp
	$(CC) $(CFLAGS) -o $@ $<