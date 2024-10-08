#include <stdio.h>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Incorrect args count");
        return -1;
    }
    printf(argv[1]);
    return 0;
}