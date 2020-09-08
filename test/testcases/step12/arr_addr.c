int a[4][4];
int main() {
    if ((int*) a != &a[0][0]) return 1;
    if ((int)(int*) a != (int)(int*)a[0]) return 2;
    if (&a[1][0] != (int*)a[1]) return 3;
    if (&a[2][2] != &((int*)a)[2 * 4 + 2]) return 4;
    if (&a[3][3] == &((int**)a)[3][3]) return 5;
    return 0;
}
