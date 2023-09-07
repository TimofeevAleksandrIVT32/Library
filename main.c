#include <stdio.h>
#include "lib.h"

int main(int argc, char *argv[]) {
    if (argc != 2){
        printf("Invalid number of arguments\n");
        return 1;
    }
    int er = http_request(argv[1]);
    if (er) {
        return 1;
    }
    return 0;
}
