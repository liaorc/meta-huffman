#include "fix_string.h"

int main() {
    MAKESTRINGTINY("myhelloworld") a3;
    a3.InfoPrint();

    MAKESTRINGTINY("1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0987654321") a5;
    a5.InfoPrint();
    return 0;
}
