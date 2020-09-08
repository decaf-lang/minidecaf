int main() {
    int a; int b;

    int * p1 = &a; int* p2 = &b;
    int *p3 = &a;
    return (p1==p3)*100 + (p2==p3)*10 + 3;
}
