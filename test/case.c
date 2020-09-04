int main() { int x=3; int y=5; return foo(&x, y); } int foo(int *x, int y) { return *x + y; }
