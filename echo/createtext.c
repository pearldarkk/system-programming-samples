#include <stdio.h>
#include <string.h>

int main() {
    freopen("text.txt", "wb", stdout);
    int j = 0;
    for (unsigned char i = 0;; ++i, ++j) {
        if (j == 2000)
            break;
        fputc(i, stdout);
        if (i == 255)
            i = 0;
    }
    return 0;
}