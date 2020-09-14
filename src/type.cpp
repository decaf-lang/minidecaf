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

TYPtr arr_of(TYPtr base, int arr_len) {
    TYPtr ty = std::make_shared<Type>();
    ty->kind = TY_ARR;
    ty->align = 4;
    ty->size = arr_len * base->size;
    ty->base = base;
    ty->arr_len = arr_len;
    ty->elem_size = base->size;
    ty->pointer_depth = base->pointer_depth + 1;
    ty->arr_dim = base->arr_dim + 1;
    return ty;
}

bool type_equal(TYPtr a, TYPtr b) {
    assert(a && b);
    bool rst = a->kind == b->kind;
    while(rst && a->kind != TY_INT) {
        if(a->kind == TY_ARR) {
           rst = (a->arr_len == b->arr_len);
           if(!rst) break;
        }
        a = a->base;
        b = b->base;
        assert(a && b);
        rst = a->kind == b->kind;
    }
    return rst;
}