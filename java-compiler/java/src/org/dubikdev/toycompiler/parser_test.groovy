package org.dubikdev.toycompiler;

import org.antlr.runtime.ANTLRStringStream
import org.antlr.runtime.CommonTokenStream
import org.antlr.runtime.RecognitionException
import org.antlr.runtime.tree.CommonTree
import groovy.json.JsonBuilder

import org.antlr.runtime.Token
import static org.dubikdev.toycompiler.RppLexer.*
import groovy.json.JsonSlurper

class GroovyParserTest extends GroovyTestCase {

    GroovyParserTest() {
    }

    @Override
    protected void setUp() {
    }

    /*
    public void testFunctionDeclaration() throws Exception {
        def code = """
        def func(x)
            x = 0
        end
        """

        def tree = parse(code);
        def token = tree.getToken();

        assertEquals(token.getType(), FUNC_DEF);

        def ar = toArray(tree)

        def children = tree.getChildren();
        assertEquals("func", children[0].getToken().getText())
        assertEquals(ARG_DEF, children[1].getToken().getType())
        assertEquals(BLOCK, children[2].getToken().getType())
    }
    */

    List toArray(CommonTree tree) {
        def array = []
        return toArrayRecursively(tree)
    }

    class TokenInfo {
        public String text
        public int type

        String toString() {
            return text + ", " + type
        }
    }

    List toArrayRecursively(CommonTree tree) {
        def array = []
        array.add(getTokenInfo(tree.getToken()))

        if (tree.getChildren() != null) {
            for (child in tree.getChildren()) {
                def children = []
                children = toArrayRecursively(child)
                array.add(children)
            }
        }

        return array
    }

    TokenInfo getTokenInfo(Token token) {
        TokenInfo info = new TokenInfo()
        info.text = token.getText()
        info.type = token.getType()
        return info
    }

    public void testArray() {
        def one = [10, 5]
        assertEquals([10, 5], one)
    }

    public void testNestedArrays() {
        def one = [10, [5, 4]]
        assertEquals([10, [5, 4]], one)

        ["+", ["10", "5"]]

    }

    // x + y
    // [+, [x, y]]

    public void testSimpleExpressionTwoNumbersAdd() {
        def code = """
        10+5
        """

        def tree = parse(code)
        //assertNotNull("Was not able to parse following code: " + code, tree)
        //def token = tree.getToken()

        assert true
    }

    private CommonTree parse(String inputString) throws RecognitionException {
        def inputStream = new ANTLRStringStream(inputString);
        def lexer = new RppLexer(inputStream);
        def tokenStream = new CommonTokenStream(lexer);
        def parser = new RppParser(tokenStream);
        return (CommonTree) parser.prog().getTree();
    }
}
