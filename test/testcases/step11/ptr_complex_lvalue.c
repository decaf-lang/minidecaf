int *f(int *a) {
    return a;
}
int main(){
    int a = 6;
    *f(&a)=7;
    return a;
}
