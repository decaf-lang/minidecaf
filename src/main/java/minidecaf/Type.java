package minidecaf;

/**
 * MiniDecaf 的类型（无类型、整型）
 *
 * @author  Namasikanam
 * @since   2020-09-11
 */
public abstract class Type {
    private final String name;

    private Type(String name) {
        this.name = name;
    }

    @Override
    public String toString() {
        return name;
    }

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
    }

    /**
     * 整型
     */
    public static class IntType extends Type {
        public IntType() {
            super("IntType");
        }

        @Override
        public boolean equals(Type type) {
            return type instanceof IntType;
        }

        @Override
        public int getSize() {
            return 4;
        }
    }
}