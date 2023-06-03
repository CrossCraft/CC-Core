#include <stdio.h>
#include <CC/core.h>

void CC_Core_Init(void) {
    fprintf(stdout, "CC_Core_Init\n");

    CC_World_Init(256, 64, 256);
    CC_World_Generate();
}

void CC_Core_Term(void) {
    CC_World_Term();
}
