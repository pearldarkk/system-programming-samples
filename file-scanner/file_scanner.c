#define _XOPEN_SOURCE 500 /* Get nftw() declaration */
#include <dirent.h>
#include <ftw.h>
#include <stdbool.h>

#include "tlpi_hdr.h"

char *keyword;
char cwd[512];
bool isCurrent;

static void usageError(const char *progName, const char *msg) {
    if (msg != NULL)
        fprintf(stderr, "%s\n", msg);
    fprintf(stderr, "Usage: %s [-r] filename [directory-path]\n", progName);
    fprintf(stderr, "\t-r: recursive scanning\n");
    fprintf(stderr, "\tif no directory path was specified, current directory will be used\n");
    exit(EXIT_FAILURE);
}

/* Function called by nftw() */
static int dirTree(const char *pathname, const struct stat *sbuf, int type, struct FTW *ftwb) {
    if (type != FTW_F)  // if is not file
        return 0;       // continue

    // if filename match
    if (strstr(&pathname[ftwb->base], keyword))
        printf("%s\n", pathname);
    return 0; /* Tell nftw() to continue */
}

// scan in dir
static int scanFile(const char *dirpath) {
    DIR *dirp;
    struct dirent *dp;
    if ((dirp = opendir(dirpath)) == NULL) {
        fprintf(stderr, "failed to open %s", dirpath);
        return 1;
    }
    while ((dp = readdir(dirp)) != NULL) {
        // regular file
        if (dp->d_type == 8)
            if (strstr(dp->d_name, keyword))
                printf("%s/%s\n", dirpath, dp->d_name);
    }
    if (closedir(dirp) == -1) {
        fprintf(stderr, "failed to close dir\n");
        return 1;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2 || argc > 4) {
        usageError(argv[0], NULL);
        exit(EXIT_FAILURE);
    }

    int opt;
    opt = getopt(argc, argv, "r");
    keyword = argv[optind];  // filename
    isCurrent = 0;
    if (argc <= optind + 1) {  // use cwd
        getcwd(cwd, sizeof cwd);
        isCurrent = 1;
    }
    // if recursive flag is set
    if (opt == 'r') {
        // if filepath is not specified, use cwd
        if (nftw((argc > optind + 1) ? argv[optind + 1] : cwd, dirTree, 10, 0) == -1) {
            perror("nftw");
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    } else {
        exit(scanFile((argc > optind + 1) ? argv[optind + 1] : cwd));
    }
}