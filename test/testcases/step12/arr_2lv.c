int main() {
    int a[2][2];
    int k = 0;
    for (int i = 0; i < 2; i = i + 1)
        for (int j = 0; j < 2; j = j + 1)
            a[i][j] = k=k+1;
    return a[0][0]*40 + a[0][1] * 20 + a[1][0] * 10 + a[1][1];
}
