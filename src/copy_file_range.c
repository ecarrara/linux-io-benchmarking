#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <unistd.h>

#define BUFFERSIZE (1024 * 16)

inline static
ssize_t copy_file_range(int fd_in, loff_t *off_in,
                        int fd_out, loff_t *off_out,
                        size_t len, unsigned int flags) {
    return syscall(__NR_copy_file_range, fd_in, off_in,
                   fd_out, off_out, len, flags);
}

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

    struct stat stat;
    if (fstat(fd_in, &stat) == -1) {
        perror("fstat");
        return 1;
    }

    ssize_t len = stat.st_size;

    int fd_out = open(argv[2], O_CREAT | O_WRONLY | O_TRUNC | O_DIRECT | O_SYNC, 0644);
    if (fd_out == -1) {
        perror("open (argv[2])");
        return 1;
    }

    do {
        ssize_t copied = copy_file_range(fd_in, NULL, fd_out, NULL, BUFFERSIZE, 0);
        if (copied == -1) {
            perror("copy_file_range");
            return 1;
        }

        len -= copied;
    } while (len > 0);

    close(fd_in);
    close(fd_out);

    return 0;
}
