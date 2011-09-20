package org.dubikdev.toycompiler;

import junit.framework.TestCase;
import org.antlr.runtime.ANTLRStringStream;
import org.antlr.runtime.CommonTokenStream;
import org.antlr.runtime.RecognitionException;
import org.antlr.runtime.Token;
import org.antlr.runtime.tree.CommonTree;

public class ParserTest extends TestCase {
    public ParserTest() {
    }

    public void testFunctionDeclaration() throws Exception {
        CommonTree t = parse("def func(x)\rx = 0\rend");
        Token token = t.getToken();
        assertEquals(token.getType(), rppLexer.FUNC_DEF);
    }

    private CommonTree parse(String inputString) throws RecognitionException {
        ANTLRStringStream inputStream = new ANTLRStringStream(inputString);
        rppLexer lexer = new rppLexer(inputStream);
        CommonTokenStream tokenStream = new CommonTokenStream(lexer);
        rppParser parser = new rppParser(tokenStream);
        return (CommonTree) parser.prog().getTree();
    }
}
