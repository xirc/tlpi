#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define RECORD_SIZE 8096


static int
after(const struct timespec *old, unsigned long elapsed_ms, struct timespec *new)
{
    unsigned long elapsed_sec, elapsed_nsec;

    elapsed_sec = elapsed_ms / 1000;
    elapsed_nsec = (elapsed_ms - elapsed_sec * 1000) * 1000;

    new->tv_sec = old->tv_sec + elapsed_sec;
    new->tv_nsec = old->tv_nsec + elapsed_nsec;
    if (new->tv_nsec >= 1000000000) {
        new->tv_nsec -= 1000000000;
        new->tv_sec += 1;
    }

    return 0;
}


static int
parse_record(const char *buf, size_t buflen,
             unsigned long *timestamp, char *str, size_t len)
{
    size_t bsz, ssz;
    char *ps, *pb;

    *timestamp = strtoul(buf, &pb, 0);
    if (*pb != ' ') {
        return -1;
    }
    pb++;
    bsz = buflen - (pb - buf);

    ps = str;
    ssz = len;
    while (bsz > 0 && ssz > 0) {
        if (bsz >= 2 &&
            *pb == '\\' && *(pb+1) == 'n')
        {
            *ps++ = '\n';
            ssz--;
            pb += 2;
            bsz -= 2;
        } else {
            *ps++ = *pb++;
            ssz--; bsz--;
        }
    }

    return len - ssz;
}


int
main(int argc, char *argv[])
{
    FILE *sfp;
    char buf[RECORD_SIZE];
    int buflen;
    char string[RECORD_SIZE];
    int string_len;
    unsigned long timestamp;
    struct timespec begin, next;

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "usage: %s typescript.timed\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    sfp = fopen(argv[1], "r");
    if (sfp == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    if (clock_gettime(CLOCK_MONOTONIC, &begin) == -1) {
        perror("clock_gettime");
        exit(EXIT_FAILURE);
    }

    printf("# replay start!\n");
    while (fgets(buf, RECORD_SIZE, sfp) != NULL) {
        buflen = strlen(buf);
        if (buf[buflen-1] == '\n') {
            buf[buflen-1] = '\0';
            buflen--;
        }
        string_len = parse_record(buf, buflen, &timestamp, string, RECORD_SIZE);
        if (string_len == -1) {
            fprintf(stderr, "Failed to parse record\n");
            exit(EXIT_FAILURE);
        }

        after(&begin, timestamp, &next);
        while (clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL) != 0) {
            continue;
        }

        if (write(STDOUT_FILENO, string, string_len) == -1) {
            perror("write");
            exit(EXIT_FAILURE);
        }
    }
    printf("# replay end\n");

    exit(EXIT_SUCCESS);
}
