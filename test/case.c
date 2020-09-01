int foo = 0; int fun1() { foo = 3; return 0;} int fun2() { return foo;} int main() { fun1(); return fun2();}
