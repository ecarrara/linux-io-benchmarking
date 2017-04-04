#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <unistd.h>

#define BUFFERSIZE (1024 * 16)

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <source> <destination>\n", argv[0]);
        return 1;
    }

    int fd_in = open(argv[1], O_RDONLY | O_SYNC);
    if (fd_in == -1) {
        perror("open (argv[1])");
        return 1;
    }

    int fd_out = open(argv[2], O_CREAT | O_WRONLY | O_TRUNC | O_SYNC, 0644);
    if (fd_out == -1) {
        perror("open (argv[2])");
        return 1;
    }

    char buffer[BUFFERSIZE];
    for (;;) {
        ssize_t read_ = read(fd_in, &buffer, BUFFERSIZE);
        if (read_ == 0) break;

        if (read_ == -1) {
            perror("read");
            return 1;
        }

        ssize_t total_written = 0;
        while (total_written < read_) {
            ssize_t written = write(fd_out, buffer + total_written, read_);
            if (written == -1) {
                perror("write");
                return 1;
            }
            total_written += written;
        }
    }

    close(fd_in);
    close(fd_out);

    return 0;
}
