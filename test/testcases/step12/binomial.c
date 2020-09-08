int C[2][10000];
int P = 10000007;

int swap(int **p, int **q) {
    int *t = *p;
    *p = *q;
    *q = t;
}

int main() {
    int n = 5996;
    int *p = (int *)C[0];
    int *q = (int *)C[1];

    p[0] = 1;
    for (int i = 1; i <= n; i = i + 1) {
        for (int j = 0; j <= i; j = j + 1)
            q[j] = !j ? 1 : (p[j] + p[j - 1]) % P;
        swap(&p, &q);
    }

    if (p[0] != 1 || p[n] != 1 || q[1] != n - 1) return 1;
    if (p[1234] != 6188476) return 2;
    if (p[2333] != 9957662) return 3;
    if (p[3456] != 9832509) return 4;
    if (p[5678] != 2436480) return 5;
    if (p[n / 2] != 7609783) return 6;
    return 0;
}
