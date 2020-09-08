int main() {
    int a = 2;
    int *b = &a;
    int **c = &b;
    int ***d = &c;
    int ****e = &d;
    int *****f = &e;
    (*****f) = 23;
    *(&*(&*&*&(*(&(***(**&e)))))) = **(**(**(&(f)))) + 1;
    return a;
}
