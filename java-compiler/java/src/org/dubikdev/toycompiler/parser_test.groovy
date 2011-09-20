package org.dubikdev.toycompiler;

import org.antlr.runtime.ANTLRStringStream
import org.antlr.runtime.CommonTokenStream
import org.antlr.runtime.RecognitionException
import org.antlr.runtime.tree.CommonTree

class GroovyParserTest extends GroovyTestCase {

    GroovyParserTest() {
    }

    @Override
    protected void setUp() {
    }

    public void testFunctionDeclaration() throws Exception {
        def code = """
        def func(x)
            x = 0
        end
        """

        def tree = parse(code);
        def token = tree.getToken();
        assertEquals(token.getType(), rppLexer.FUNC_DEF);
    }

    private CommonTree parse(String inputString) throws RecognitionException {
        def inputStream = new ANTLRStringStream(inputString);
        def lexer = new rppLexer(inputStream);
        def tokenStream = new CommonTokenStream(lexer);
        def parser = new rppParser(tokenStream);
        return (CommonTree) parser.prog().getTree();
    }
}
