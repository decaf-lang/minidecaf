#include <cstdlib>
#include "type.h"

Type INT_TYPE = {TY_INT, 4, 4, NULL};

TYPtr INT_TYPE_PTR(&INT_TYPE);

TYPtr pointer_to(TYPtr base) {
    TYPtr ty = std::make_shared<Type>();
    ty->kind = TY_PTR;
    ty->align = ty->size = 4;
    ty->base = base;
    return ty;
}

bool type_equal(TYPtr a, TYPtr b) {
    assert(a && b);
    bool rst = a->kind == b->kind;
    while(rst && a->kind == TY_PTR) {
        a = a->base;
        b = b->base;
        assert(a && b);
        rst = a->kind == b->kind;
    }
    return rst;
}