int N = 13;

int fib(int n) {
    if (n <= 1) {
        return n;
    }
    return fib(n - 1) + fib(n - 2);
}

int calc_fib(int n, int* f) {
    f[0] = 0; f[1] = 1;
    for (int i = 2; i <= n; i = i + 1)
        f[i] = f[i - 1] + f[i - 2];
}

int main() {
    int f[100];
    calc_fib(N, (int*) f);
    if (f[N] != fib(N))
        return -1;
    return f[N];
}
