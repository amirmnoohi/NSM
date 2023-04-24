#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define DEVICE_PATH "/dev/NSM"
#define BUFFER_SIZE 1024

int main()
{
    int fd, ret;
    char buffer[BUFFER_SIZE];

    // Open the device file
    fd = open(DEVICE_PATH, O_RDWR);

    if (fd < 0) {
        perror("Failed to open device file");
        return 1;
    }

    // Write some data to the device
    ret = write(fd, "Hello, NSM!", 12);

    if (ret < 0) {
        perror("Failed to write to device");
        return 1;
    }

    // Read some data from the device
    ret = read(fd, buffer, BUFFER_SIZE);

    if (ret < 0) {
        perror("Failed to read from device");
        return 1;
    }

    // Print the data that was read
    printf("Data read from device: %s\n", buffer);

    // Close the device file
    close(fd);

    return 0;
}
