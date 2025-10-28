#include <stdio.h>

// Opens an I2C bus and returns the file descriptor
int i2c_open(const char *bus, int addr);
// Sends data over I2C
int i2c_write(int fd, const unsigned char *data, ssize_t length);
// Reads data over I2C
int i2c_read(int fd, unsigned char *buffer, ssize_t length);
// writes then reads
int i2c_write_then_read(int fd, unsigned char *in, size_t in_length, unsigned char *out, size_t out_length);
