#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <errno.h>

#define BUFFERSIZE (1024 * 16)

#define min(a, b) ((a) < (b) ? (a) : (b))

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <source> <destination>\n", argv[0]);
        return 1;
    }

    int fd_in = open(argv[1], O_RDONLY | O_DIRECT | O_SYNC);
    if (fd_in == -1) {
        perror("open (argv[1])");
        return 1;
    }

    int fd_out = open(argv[2], O_CREAT | O_WRONLY | O_TRUNC | O_DIRECT | O_SYNC, 0644);
    if (fd_out == -1) {
        perror("open (argv[2])");
        return 1;
    }

    struct stat stat;
    if (fstat(fd_in, &stat) == -1) {
        perror("fstat");
        return 1;
    }

    ssize_t len = stat.st_size;

    static int pipes[2];
    if (pipe(pipes) == -1) {
        perror("pipe");
        return 1;
    }

    ssize_t total_written = 0;
    while (total_written < len) {
        ssize_t read = splice(fd_in, NULL, pipes[1], NULL,
                              min(len - total_written, BUFFERSIZE),
                              SPLICE_F_MOVE | SPLICE_F_MORE);

        if (read == -1) {
            if (errno == EINTR || errno == EAGAIN) {
                continue;
            }

            perror("splice read");
            return 1;
        }

        while (read > 0) {
            ssize_t written = splice(pipes[0], NULL, fd_out, NULL,
                                     read, SPLICE_F_MOVE | SPLICE_F_MORE);
            if (written == -1) {
                if (errno == EINTR || errno == EAGAIN) {
                    continue;
                }

                perror("splice write");
                return 1;
            }
            read -= written;
            total_written += written;
        }
    }
    close(fd_in);
    close(fd_out);

    return 0;
}
