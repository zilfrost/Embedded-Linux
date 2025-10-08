#include "strutils.h"
#include <stdio.h>

int main() {
    char text[] = "   test   ";

    printf("Original: '%s'\n", text);

    // Test trimming function
    str_trim(text);
    printf("After trim: '%s'\n", text);

    // Test reverse function
    str_reverse(text);
    printf("After reverse: '%s'\n", text);

    // Test string-to-int conversion
    const char *numStr = "  -12345 ";
    int number = str_to_int(numStr);
    printf("Converted number: %d\n", number);

    return 0;
}
