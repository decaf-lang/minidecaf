package minidecaf;

import java.util.List;

/**
 * 函数类型
 *
 * @author  Namasikanam
 * @since   2020-09-11
 */
public class FunType {
    public final Type returnType;
    public final List<Type> paramTypes;

    public FunType(Type returnType, List<Type> paramTypes) {
        this.returnType = returnType;
        this.paramTypes = paramTypes;
    }

    public boolean equals(FunType funType) {
        if (!returnType.equals(funType.returnType)) return false;
        if (paramTypes.size() != funType.paramTypes.size()) return false;
        for (int i = 0; i < paramTypes.size(); ++i)
            if (!paramTypes.get(i).equals(funType.paramTypes.get(i)))
                return false;
        return true;
    }
}
