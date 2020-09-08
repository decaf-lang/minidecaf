int a[10];
int k;
int main() {
    k=0;
    while (k<10) {
        a[k]=k;
        k=k+1;
    }

    int *b[10];
    k=0;
    while (k<10) {
        b[k]=&a[k];
        k=k+1;
    }
    int **cb = (int**)b;
    int **c[10];

    k = 0;
    while (k<10) {
        for (int i=0; i+k<10; i=i+1) {
            if (cb[k][i] != k + i)
                return 100 + k*10 + i;
        }
        k=k+1;
    }

    k=0;
    while (k<10) {
        c[k]=cb;
        cb = cb + 1;
        k=k+1;
    }

    k=0;
    while (k<10) {
        if (**c[k] != a[k])
            return 1;
        k=k+1;
    }
    return 0;
}
