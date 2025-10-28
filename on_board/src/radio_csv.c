#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include "i2c_helper.h"
#include "keylisten_helper.h"


#define REG_COUNT 4
#define READ_SIZE 2
#define MAX_NAME_LENGTH 24

typedef struct {
    char name[MAX_NAME_LENGTH];
    unsigned char address[1];
    unsigned int bitstoread;
} reg;

unsigned int convert_bytes_to_uint32(unsigned char *bytes, unsigned int bits_used){
    unsigned char mask = 0;
    unsigned int byte_index = 0;
    unsigned int result = 0;

    // set the mask to be the last byte of the bits used.
    // If 11 bits are used (mask of 0000 0111 1111 1111) then mask will be 0000 0111
    for (unsigned int i = 0; i < bits_used % 8; i++){
        mask |= (1 << i);
    }
    result = (bytes[0] & mask);
    if (bits_used % 8 == 0){
        bits_used -= 8;
    } else {
        bits_used -= (bits_used % 8);
    }

    byte_index += 1;
    while (bits_used > 0) {
        result <<= 8;
        result |= bytes[byte_index];

        byte_index += 1;
        bits_used -= 8;
    }

    return result;
}

int write_reg_names_to_file(FILE *f, reg *regs, int reg_count){

    for (int i = 0; i < reg_count; i++) {
        if (i > 0){
            fprintf(f, ", ");
        }
        fprintf(f, "%s", regs[i].name);
    }
    fprintf(f, "\n");
    return 0;

}
int write_reg_values_to_file(FILE *f, int i2c_fd, reg *regs, int reg_count){
    unsigned char rx_buffer[READ_SIZE];

    for (int i = 0; i < reg_count; i++) {
	if (i2c_write_then_read(i2c_fd, regs[i].address, 1, rx_buffer, sizeof(rx_buffer)) < 0){
	    return 1;
	}
	if (i > 0){
	    fprintf(f, ", ");
	}
	unsigned int result = convert_bytes_to_uint32(rx_buffer, regs[i].bitstoread);
	if (strcmp(regs[i].name, "RSSI") == 0){
	    fprintf(f, "%f", result*3.0/4096);
	} else {
	    fprintf(f, "%d", result);
	}
    }
    fflush(f);
    return 0;
}

int main() {
    const char *i2c_bus = "/dev/i2c-1";
    int i2c_addr = 0x25;
    int fd;

    // Open I2C bus and set slave address
    fd = i2c_open(i2c_bus, i2c_addr);
    if (fd < 0) return 1;


    reg init_registers[] = {{.name = "RX Freqency", .address = {0x07}, .bitstoread = 11}};
    reg loop_registers[] = {
        {.name = "CRC fail", .address = {0x21}, .bitstoread = 16},
        {.name = "RX count", .address = {0x23}, .bitstoread = 16},
        {.name = "RX fail full", .address = {0x25}, .bitstoread = 8},
        // {.name = "RSSI", .address = {0x2A}, .bitstoread = 12},
    };

    unsigned char rx_buffer[READ_SIZE];

    if (i2c_write_then_read(fd, init_registers[0].address, 1, rx_buffer, sizeof(rx_buffer)) < 0){
        close(fd);
        return 1;
    }

    float rx_frequency = convert_bytes_to_uint32(rx_buffer,11) * 12.5 / 1000 + 399.9;

    printf("Press q to stop\n");
    printf("Receiving at frequency %f MHz\n", rx_frequency);

    write_reg_names_to_file(stdout, loop_registers, REG_COUNT);

    enable_raw_mode(); // set raw mode to listen for key presses to quit

    while(1){
        printf("\x1b[0J");
        write_reg_values_to_file(stdout, fd, loop_registers, REG_COUNT);
        if (kbhit()) {
            char c = getchar();
            if (c == 'q') {
                printf("\nQuitting loop.\n");
                break;
            }
        }
        printf("\r");
        usleep(250000);
    }
    disable_raw_mode(); // turn off raw mode
    printf("Enter filename to save these values to (empty to ignore): ");

    char filename[128];

    if (fgets(filename, sizeof(filename), stdin) != NULL) {
    // Remove the trailing newline character
    filename[strcspn(filename, "\n")] = 0;

    if (strlen(filename) > 0) {
        FILE *outfile = fopen(filename, "w");

        write_reg_names_to_file(outfile, loop_registers, REG_COUNT);
        write_reg_values_to_file(outfile, fd, loop_registers, REG_COUNT);
        fprintf(outfile, "\n");

        fclose(outfile);
    }
    }
    close(fd);
    return 0;
}

