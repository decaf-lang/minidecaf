int main() {
    int a; int b; int c;
    a=0;b=0;c=0;
    int* arr[3];
    arr[0]=&a;
    arr[1]=&b;
    arr[2]=&c;
    c=1+ (b=1+ (a=a+1));
    return *arr[0] * 2 + *arr[1] * 3  + *arr[2] * 4;
}
