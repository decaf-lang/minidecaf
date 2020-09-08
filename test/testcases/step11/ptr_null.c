int g=123;
int *gp;
int main() {
    int a=456;
    int b=789;
    int *np=(int *)0;
    np=( int  * )(int * *) - (int) 0;
    if (np != (int*)0) return 244;
    if (np != gp) return 244;
    if (np == &a) return 244;
    if (np == &g) return 244;
    if (&a == &g) return 244;
    if (&a == &b) return 244;
    if (&b == &g) return 244;
    if (np == (int*)0) a=23;
    np=&a;
    return*np;
}
