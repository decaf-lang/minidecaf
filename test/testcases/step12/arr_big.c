int a[1048576];
int main() {
    for (int i =0;i<1048576;i=1+i)
        a[i] = 1048576-i;
    return a[142123] + a[564432];
}
