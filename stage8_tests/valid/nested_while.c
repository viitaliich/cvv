int main() {
    int a = 1;

    while (a / 2 < 2) {
        int b = 1;
        while (b < 10)
            b = b*2;
        a = a + b;
    }

    return a;
}