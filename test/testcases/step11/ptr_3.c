int *f(int *a) {
    return a;
}
int main(){
    int a = 6;
    int *p = f(&a);
    *p=7;
    return a;
}
