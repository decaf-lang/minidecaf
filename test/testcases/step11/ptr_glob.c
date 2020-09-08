int a=23;
int main() {
    int *p = &a;
    *p = 2;
    return a;
}
