int b;
int main() {
    int a;
    int x;
    a=2;b=3;

    int *c = &a;
    int *d = &b;
    int *e = c; c = d; d = e;
    e=&x;

    return *c*10 + *d;
}
