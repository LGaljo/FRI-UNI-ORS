#include <stdio.h>

typedef unsigned int uint;

uint enice(uint n) {
    //  Na desni strani števila je n enic
    if (n >= 32) return 0xFFFFFFFF;
    return ((1 << n) - 1);
}

uint vrniBite(uint x, uint p, uint n) {
    uint b = enice(n);
    x = x & (b << p);
    x = x >> p;
    return x;
}

uint nastaviBite(uint x, uint p, uint n) {
    uint b = enice(n);
    x = x | (b << p);
    return x;
}

int main() {
    uint m = 0xAB;
    printf("0x%04X\n", enice(5));
    // 0xAB =
    printf("%u\n", vrniBite(m, 2, 4));
    printf("%u\n", nastaviBite(m, 2, 4));
    return 0;
}