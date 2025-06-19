#include <stdio.h>

// Direct recursive function
int direct_recursive(int n) {
    if (n <= 0) return 0;
    return n + direct_recursive(n - 1);
}

// Indirect recursive functions
int indirect_recursive_a(int n);
int indirect_recursive_b(int n);

int indirect_recursive_a(int n) {
    if (n <= 0) return 0;
    return n + indirect_recursive_b(n - 1);
}

int indirect_recursive_b(int n) {
    if (n <= 0) return 0;
    return n + indirect_recursive_a(n - 1);
}

// Non-recursive function
int non_recursive(int n) {
    return n * 2;
}

// int main() {
//     printf("Direct recursive: %d\n", direct_recursive(5));
//     printf("Indirect recursive a: %d\n", indirect_recursive_a(5));
//     printf("Non-recursive: %d\n", non_recursive(5));
//     return 0;
// }