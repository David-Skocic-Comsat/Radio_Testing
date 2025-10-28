#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include "i2c_helper.h"

// Opens an I2C bus and returns the file descriptor
int i2c_open(const char *bus, int addr) {
    int fd = open(bus, O_RDWR);
    if (fd < 0) {
        perror("Failed to open the I2C bus");
        return -1;
    }

    if (ioctl(fd, I2C_SLAVE, addr) < 0) {
        perror("Failed to set I2C slave address");
        close(fd);
        return -1;
    }

    return fd;
}

// Sends data over I2C
int i2c_write(int fd, const unsigned char *data, ssize_t length) {
    if (write(fd, data, length) != length) {
        perror("Failed to write to I2C device");
        return -1;
    }
    return 0;
}

// Reads data over I2C
int i2c_read(int fd, unsigned char *buffer, ssize_t length) {
    if (read(fd, buffer, length) != length) {
        perror("Failed to read from I2C device");
        return -1;
    }
    return 0;
}

// writes a register (in) and fills buffer with data (out)
int i2c_write_then_read(int fd, unsigned char *in, size_t in_length, unsigned char *out, size_t out_length){
    int ret = 0;
    if ((ret = i2c_write(fd, in, in_length)) < 0) {
        return ret;
    }
    if ((ret = i2c_read(fd, out, out_length)) < 0) {
        return ret;
    }
    return ret;
}
