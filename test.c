#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <errno.h>

#include <sys/stat.h>
#include <sys/types.h>


int main(int argc, char *argv[]) {
    char results[256] = {0};
    int fd = open("/dev/reverse", O_RDWR);
    if (fd < 0) 
    { 
        perror("Error: Reverse");
        return -1;
    }
    printf("Input : %s\n", argv[1]);
    write(fd, argv[1], strlen(argv[1]));
    read(fd, results, strlen(argv[1]));
    printf("Output: %s\n", results);
    close(fd);

    return 0;
}
