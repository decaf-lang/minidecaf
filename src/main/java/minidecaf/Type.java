package minidecaf;

import java.util.*;

/**
 * MiniDecaf 的类型（无类型、整型、指针、数组）
 *
 * @author  Namasikanam
 * @since   2020-09-11
 */
public abstract class Type {
    private final String name;

    // 值类别：左值或者右值
    enum ValueCat {
        LVALUE,
        RVALUE
    }

    public final ValueCat valueCat; // 默认是右值

    private Type(String name) {
        this.name = name;
        this.valueCat = ValueCat.RVALUE;
    }

    private Type(String name, ValueCat valueCat) {
        this.name = name;
        this.valueCat = valueCat;
    }

    @Override
    public String toString() {
        return name + "(" + valueCat + ")";
    }

    abstract public Type referenced(); // 取地址

    abstract public Type dereferenced(); // 解引用

    abstract public Type valueCast(ValueCat targetValueCat); // 在左右值之间转换

    abstract public boolean equals(Type type); // 判断两个 Type 是否相等

    abstract public int getSize(); // 类型所占内存空间的大小

    /**
     * 用于语句、声明等没有类型的分析树节点
     */ 
    public static class NoType extends Type {
        public NoType() {
            super("NoType");
        }

        @Override
        public boolean equals(Type type) {
            return type instanceof NoType;
        }

        @Override
        public int getSize() {
            throw new UnsupportedOperationException("Error: trying getting the size of NoType.");
        }
        
        @Override
        public Type referenced() {
            throw new UnsupportedOperationException("Error: trying referencing NoType.");
        }

        @Override
        public Type dereferenced() {
            throw new UnsupportedOperationException("Error: trying dereferencing NoType.");
        }

        // value cast 对于 NoType 而言实际上是无意义的
        @Override
        public Type valueCast(ValueCat targetValueCat) {
            return this;
        }
    }

    /**
     * 整型
     */
    public static class IntType extends Type {
        public IntType() {
            super("IntType");
        }

        public IntType(ValueCat valueCat) {
            super("IntType", valueCat);
        }

        @Override
        public boolean equals(Type type) {
            return (type instanceof IntType) && valueCat == type.valueCat;
        }

        @Override
        public int getSize() {
            return 4;
        }

        @Override
        public Type referenced() {
            if (valueCat == ValueCat.LVALUE) return new PointerType(1);
            else throw new UnsupportedOperationException("Error: trying referencing an rvalue int");
        }

        @Override
        public Type dereferenced() {
            throw new UnsupportedOperationException("Error: trying dereferencing int.");
        }

        @Override
        public Type valueCast(ValueCat targetValueCat) {
            return new IntType(targetValueCat);
        }
    }

    /**
     * 指针类型
     */
    public static class PointerType extends Type {
        public final int starNum; // 有多少个"*"，比如说 int ** 的 starNum 为 2。

        public PointerType(int starNum) {
            super("PointerType<" + starNum + ">");
            this.starNum = starNum;
        }

        public PointerType(int starNum, ValueCat valueCat) {
            super("PointerType<" + starNum + ">", valueCat);
            this.starNum = starNum;
        }

        @Override
        public boolean equals(Type type) {
            return type instanceof PointerType && starNum == ((PointerType)type).starNum && valueCat == type.valueCat;
        }

        @Override
        public int getSize() {
            return 4;
        }

        @Override
        public Type referenced() {
            if (valueCat == ValueCat.LVALUE) return new PointerType(starNum + 1);
            else throw new UnsupportedOperationException("Error: trying referencing an rvalue pointer");
        }

        @Override
        public Type dereferenced() {
            if (starNum > 1) return new PointerType(starNum - 1, ValueCat.LVALUE);
            else return new IntType(ValueCat.LVALUE);
        }

        @Override
        public Type valueCast(ValueCat targetValueCat) {
            return new PointerType(starNum, targetValueCat);
        }
    }

    /**
     * 数组类型
     *
     * 这是一个非常特殊的类型
     */
     public static class ArrayType extends Type {
        final public Type baseType;
        final private int size;

        public ArrayType(Type baseType, int length) {
            super("ArrayType<" + length + ":" + baseType + ">", ValueCat.RVALUE);
            this.baseType = baseType;
            this.size = length * baseType.getSize();
        }

        @Override
        public boolean equals(Type type) {
            return type instanceof ArrayType && size == type.getSize() && baseType.equals(((ArrayType)type).baseType);
        }

        @Override
        public Type referenced() {
            throw new UnsupportedOperationException("Error: trying referencing array.");
        }

        @Override
        public Type dereferenced() {
            throw new UnsupportedOperationException("Error: trying dereferencing array.");
        }

        @Override
        public Type valueCast(ValueCat targetValueCat) {
            if (targetValueCat == ValueCat.LVALUE)
                throw new UnsupportedOperationException("Error: an array must be an rvalue.");
            else return this;
        }

        @Override
        public int getSize() {
            return size;
        }
    }
}