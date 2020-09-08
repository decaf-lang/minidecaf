int rand(int *state) {
    return *state = *state * 9312967 + 282132157;
}

int swap(int *a, int *b) {
    int c=*a;
    *a=*b;
    *b=c;
}
int inc(int *sorted_before) {
    *sorted_before=*sorted_before+1;
}

int bubblesort(int sorted_after, int *a) {
    for (int i =0 ; i < sorted_after; inc(&i))
        for (int j =i+1;j<sorted_after; inc(&j))
            if (*(a + i)  > a[j])
                swap(&a[i], a + j);
}

int sorted_after=500;
int a[500];
int main() {
    int state = 218397121;
    for (int i=0; i<sorted_after; inc(&i))
        a[i] = rand(&state);

    int sorted_before=1;
    for (int i=0;i<sorted_after-1; inc(&i))
        if (a[i]>a[i+1])
            sorted_before=0;

    bubblesort(sorted_after, (int*)a);

    int sorted_after=1;
    for (int i=0;i<sorted_after-1; inc(&i))
        if (a[i]>a[i+1])
            sorted_after=0;
    return 200 + sorted_before*10 + sorted_after;
}
