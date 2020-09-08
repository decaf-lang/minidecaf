int qsort(int *a, int l, int r) {
    int i = l;
    int j = r;
    int p = a[(l+r)/2];
    int flag = 1;
    while (i <= j) {
        while (a[i] < p) i = i + 1;
        while (a[j] > p) j = j - 1;
        if (i > j) break;
        int u = a[i]; a[i] = a[j]; a[j] = u;
        i = i + 1;
        j = j - 1;
    }
    if (i < r) qsort(a, i, r);
    if (j > l) qsort(a, l, j);
}

int rand(int *state) {
    *state = *state * 5000087 + 198250529;
    return *state % 1000;
}

int initArr(int n, int *a) {
    int state = 474230941;
    int i = 0;
    while (i < n) {
        a[i] = rand(&state);
        i = i + 1;
    }
}

int isSorted(int n, int *a) {
    int i = 0;
    while (i < n-1) {
        if ( *(a+i) > *(a+i+1) )
            return 0;
        i = i + 1;
    }
    return 1;
}

int n = 1000000;
int a[1000000];

int main() {
    int* ap = (int*) a;
    initArr(n, ap);
    int sorted_before = isSorted(n, ap);
    qsort(ap, 0, n-1);
    int sorted_after = isSorted(n, ap);
    if (!(sorted_before==0 && sorted_after==1))
        return 1;
    return 0;
}
