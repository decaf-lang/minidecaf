int main() {
    int a[10];
    a[4] = 12;
    return *(((int*)a+5)-1);
}
