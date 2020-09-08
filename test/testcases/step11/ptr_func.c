int swap(int* a, int* b);
int f(int *a, int *b);
int *rand(int* state);

int main() {
    int x=2;
    int y=3;
    swap(&x, &y);
    int A = x-y;

    x=y=1;
    int B = f(&y, &y);

    int s = 0;
    rand(rand(rand(rand(&s))));
    int C = s;

    return C*10 + A*10 + B;
}

int swap(int *a, int *b) {
    int c = *a;
    *a=*b;
    *b=c;
}

int f(int *a, int *b) {
    *a=0;
    return *b;
}

int *rand(int *state) {
    *state = *state *2 + 1;
    return state;
}
