int A[10][10];
int main() {
    int a[10];
    int *p=(int*) &a[4];
    return &a[9] - p + &A[6][8] - (int*)A[2];
}
