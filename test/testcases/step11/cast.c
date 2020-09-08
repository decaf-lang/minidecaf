int main() {
    int a = 214;
    int b = (int) &a;
    int c = *(int*) b;
    return c;
}
