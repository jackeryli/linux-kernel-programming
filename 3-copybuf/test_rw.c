#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define DEVICE_FILE "/dev/jackdriver"
#define BUFFER_SIZE 1024

int main() {
    int fd;
    ssize_t ret;
    char buffer[BUFFER_SIZE];
    char input;

    // Open the device file
    fd = open(DEVICE_FILE, O_RDWR);
    if (fd < 0) {
        perror("Failed to open the device file");
        return 1;
    }

    while (1) {
        printf("Enter 'w' to write, 'r' to read, or 'e' to exit: ");
        // Read a single character from standard input
        if (scanf(" %c", &input) != 1) {
            printf("Failed to read input\n");
            close(fd);
            return 1;
        }

        if (input == 'w') {
            printf("Enter string to write: ");
            // Read input string from standard input
            if (scanf(" %[^\n]", buffer) != 1) {
                printf("Failed to read input string\n");
                close(fd);
                return 1;
            }
            // Write data to the device
            ret = write(fd, buffer, strlen(buffer));
            if (ret < 0) {
                perror("Failed to write to the device");
                close(fd);
                return 1;
            }
        } else if (input == 'r') {
            // Read data from the device
            ret = read(fd, buffer, BUFFER_SIZE);
            if (ret < 0) {
                perror("Failed to read from the device");
                close(fd);
                return 1;
            }
            
            // Print the read data
            printf("Data read from device: %s\n", buffer);
        } else if (input == 'e') {
            break;
        } else {
            printf("Invalid input\n");
        }
    }

    // Close the device file
    close(fd);

    return 0;
}
