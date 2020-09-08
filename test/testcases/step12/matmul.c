int mulMatrix(int n, int *a, int *b, int *c) {
int i; int j; int k;
    i = 0;
    while (i < n) {
        j = 0;
        while (j < n) {
            c[i*n + j] = 0;
            k = 0;
            while (k < n) {
                int old = c[i*n + j];
                c[i*n + j] = old + a[i*n + k] * b[k*n + j];
                k = k + 1;
            }
            j = j + 1;
        }
        i = i + 1;
    }
}

int initMatrix(int n, int *a) {
int i; int j; int k;
    k = 0;
    i = 0;
    while (i < 2) {
        j = 0;
        while (j < 2) {
            k = k + 1;
            *(a + i*n + j) = k;
            j = j + 1;
        }
        i = i + 1;
    }
}

int a[2][2]; int b[2][2]; int c[2][2];
int main() {
    initMatrix(2, (int*)a);
    return ((int*)a)[1];
}
