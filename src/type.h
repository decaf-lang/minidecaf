#ifndef _TYPE_H_
#define _TYPE_H_

#include <cassert>
#include <memory>

enum TypeKind {
    TY_INVAILD = 0,
    TY_INT,
    TY_PTR,
};

struct Type {
    TypeKind kind;
    int size;
    int align;
    std::shared_ptr<Type> base;
};

typedef std::shared_ptr<Type> TYPtr;

extern TYPtr INT_TYPE_PTR;
extern Type INT_TYPE;

inline TYPtr int_type() {
    return INT_TYPE_PTR;
}

TYPtr pointer_to(TYPtr base);

inline bool is_integer(TYPtr ty) {
    assert(ty);
    return ty->kind == TY_INT;
}

inline bool is_ptr(TYPtr ty) {
    assert(ty);
    bool rst = ty->kind == TY_PTR;
    if(rst) 
        assert(ty->base);
    return rst;
}

bool type_equal(TYPtr, TYPtr);

#endif // _TYPE_H_