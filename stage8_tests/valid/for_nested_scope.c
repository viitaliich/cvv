int main() {
    int i = 0;
    int j = 0;

    for (i = 100; i > 0; i = i - 1) {
        i = 0;
        j = j * 2 + i;
    }

    int k = 3;

    return j + k;
}