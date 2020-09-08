int main() {
    int a[3][4][5][6];
    int *p = (int*) a[1];
    int *q = (int*) a[1][2];
    int *r = (int*) a[1][2][3];
    int *s = &a[1][2][3][4];
    a[1][2][3][4] = 2333;
    a[1][2][0][3] = 3332;
    if (p[2*5*6 + 3*6 + 4] != 2333) return 1;
    if (q[3*6 + 4] != 2333) return 2;
    if (r[4] != 2333) return 3;
    if (s[0] != 2333) return 4;
    if (&(a[1][2][3][4]) != &p[2*5*6 + 3*6 + 4]) return 5;
    if (&((int*)a[1][2][3])[4] != &q[3*6 + 4]) return 6;
    if (&((int**)a[1][2])[3][4] != (int*)q[3] + 4) return 7;
    if (1*4*5*6 + (int*)a + 2*5*6 != s - 3*6 - 4) return 8;
    if (-4 + s - (int*)a != ((1*4 + 2)*5 + 3)*6) return 9;
    if ((s-r) + (r-q) + (q-p) != s-p) return 10;
    return 0;
}
