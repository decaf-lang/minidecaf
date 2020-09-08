int main() {
    int a[10];
    a[5] = 10;
    int *p=&a[2];
    *((p+4)-1)=45;
    return a[5];
}
