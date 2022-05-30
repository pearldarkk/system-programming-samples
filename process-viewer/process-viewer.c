#define _GNU_SOURCE
#include <ctype.h>
#include <dirent.h>
#include <limits.h>
#include <stdbool.h>
#include <sys/stat.h>

#include "tlpi_hdr.h"

static void usageError(char *progName, const char *msg) {
    if (msg != NULL)
        fprintf(stderr, "%s\n", msg);
    fprintf(stderr, "Usage: %s\n", progName);
}

int main(int argc, char **argv) {
    if (argc > 1) {
        usageError(argv[0], NULL);
        exit(EXIT_FAILURE);
    }

    printf("PID\tPPID\tCmdline\n");

    DIR *dirp = opendir("/proc");
    if (dirp == NULL) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    struct dirent *dp;
    FILE *fp;
    char path[512];
    char cmd[512];
    int ppid;
    char line[512];
    bool gotPpid;
    while ((dp = readdir(dirp)) != NULL) {
        if (dp->d_type != DT_DIR || !isdigit((unsigned char)dp->d_name[0]))
            continue;

        // format proc status path
        snprintf(path, 512, "/proc/%s/status", dp->d_name);
        if ((fp = fopen(path, "r")) == NULL)
            continue;

        gotPpid = false;
        while (!gotPpid) {
            // read line by line
            if (fgets(line, 512, fp) == NULL)
                break;

            if (strncmp(line, "PPid:", 5) == 0) {
                ppid = strtol(line + 5, NULL, 10);
                gotPpid = true;
            }
        }
        fclose(fp);

        // format proc cmdline path
        snprintf(path, 512, "/proc/%s/cmdline", dp->d_name);
        if ((fp = fopen(path, "r")) == NULL)
            continue;
        // read cmdline
        printf("%s\t%d", dp->d_name, ppid);
        if (fgets(cmd, 512, fp) != NULL)
            printf("\t%s", cmd);
        printf("\n");
    }
    exit(EXIT_SUCCESS);
}