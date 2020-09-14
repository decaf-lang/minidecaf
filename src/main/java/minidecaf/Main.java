package minidecaf;

import org.antlr.v4.runtime.*;
import org.antlr.v4.runtime.tree.*;

import java.io.FileWriter;

/**
 * 这是一个 MiniDecaf 的编译器，简便起见我们只做了单遍遍历。
 * 
 * @author  Namasikanam
 * @since    2020-09-11
 */
public class Main {
    public static void main(String[] args) throws Exception {
        if (args.length != 2
            || args[0].equals("-h")
            || args[0].equals("--help")) {
            System.out.printf("Usage: minidecaf <input minidecaf file> <output riscv assembly file>\n");
            return;
        }

        /* input file --- lexer ---> tokens
                                            --- parser ---> tree
                                                                 --- visitor ---> riscv assembly
                                                                                                 --- writer ---> output file */
        CharStream input = CharStreams.fromFileName(args[0]);

        MiniDecafLexer lexer = new MiniDecafLexer(input);
        lexer.addErrorListener(new BaseErrorListener() {
            @Override
            public void syntaxError(Recognizer<?,?> recognizer, Object offendingSymbol, int line, int charPositionInLine, String msg, RecognitionException e) {
                throw new RuntimeException("Lexer Error (" + line + "," + charPositionInLine + ")");
            }
        });
        CommonTokenStream tokens = new CommonTokenStream(lexer);

        MiniDecafParser parser = new MiniDecafParser(tokens);
        parser.setErrorHandler(new BailErrorStrategy());

        ParseTree tree = parser.prog();
        StringBuilder asm = new StringBuilder();
        MainVisitor visitor = new MainVisitor(asm);
        visitor.visit(tree);
        
        FileWriter writer = new FileWriter(args[1]);
        writer.write(asm.toString());
        writer.close();
    }
}
