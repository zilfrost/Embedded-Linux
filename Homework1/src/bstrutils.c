// Function to reverse a string in-place
void str_reverse(char *str) {
    if (!str) return;
    int len = 0;
    while (str[len] != '\0') len++;

    for (int i = 0; i < len / 2; i++) {
        char temp = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = temp;
    }
}

// Function to remove leading and trailing whitespace from a string
void str_trim(char *str) {
    if (!str) return;

    int start = 0;
    while (str[start] == ' ' || str[start] == '\t' || str[start] == '\n') start++;

    int end = 0;
    while (str[end] != '\0') end++;
    end--;

    while (end >= start && (str[end] == ' ' || str[end] == '\t' || str[end] == '\n')) {
        str[end] = '\0';
        end--;
    }

    if (start > 0) {
        int i = 0;
        while (str[start + i] != '\0') {
            str[i] = str[start + i];
            i++;
        }
        str[i] = '\0';
    }
}

// Function to convert a string to an integer (safe conversion)
int str_to_int(const char *str) {
    if (!str) return 0;

    int result = 0;
    int sign = 1;
    int i = 0;

    // Skip leading whitespace
    while (str[i] == ' ' || str[i] == '\t' || str[i] == '\n') i++;

    // Check for sign
    if (str[i] == '-') {
        sign = -1;
        i++;
    } else if (str[i] == '+') {
        i++;
    }

    // Convert characters to numbers
    while (str[i] >= '0' && str[i] <= '9') {
        result = result * 10 + (str[i] - '0');
        i++;
    }

    result *= sign;

    // Limit to int range (assuming 32-bit)
    if (result < -2147483648L || result > 2147483647L) {
        return 0; // overflow
    }

    return result;
}
