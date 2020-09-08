int main() {
    int a0=2;
    int b0=3;

    int* a1 = &a0;
    int *b1;
    b1=&b0;
    *a1=3;
    *b1=4;

    int **c2=&a1;
    *c2=&b0;
    *a1=4;
    *b1=5;

    **c2 = 6;

    return a0*10+b0;
}
