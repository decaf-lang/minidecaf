int main(){
    int a[2][3][4];
    int r = 1;
    for (int i = 0; i < 2; i = i + 1)
        for (int j = 0; j < 3; j=j+1) {
            a[i][j][0] = i*6-j*3;
            for (int k = 1; k < 4; k=k+ 1) {
                r = r * 2 + 1;
                a[i][j][k] = a[i][j][k-1] + r;
            }
        }
    return a[1][2][3] + a[0][0][2];
}
