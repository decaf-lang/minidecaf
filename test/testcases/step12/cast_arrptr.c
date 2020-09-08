int main() {
    int a[10];
    a[0] = 12;
    int *p = (int*) a;
    return *p;
}
