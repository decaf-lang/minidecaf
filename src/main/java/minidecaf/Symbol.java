package minidecaf;

/**
 * 符号（仅用于局部变量）
 *
 * @author  Namasikanam
 * @since   2020-09-11
 */
public class Symbol {
    public final String name;
    
    public final int offset;

    public final Type type;

    public Symbol(String name, int offset, Type type) {
        this.name = name;
        this.offset = offset;
        this.type = type;
    }

    @Override
    public String toString() {
        return name + "@" + type + ":" + offset;
    }
}
