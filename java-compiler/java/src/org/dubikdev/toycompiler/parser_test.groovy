package org.dubikdev.toycompiler;

import org.antlr.runtime.ANTLRStringStream
import org.antlr.runtime.CommonTokenStream
import org.antlr.runtime.RecognitionException
import org.antlr.runtime.tree.CommonTree
import groovy.json.JsonBuilder

import org.antlr.runtime.Token
import static org.dubikdev.toycompiler.RppLexer.*
import groovy.json.JsonSlurper
import org.antlr.runtime.tree.CommonTreeAdaptor
import org.antlr.runtime.tree.TreeAdaptor

class GroovyParserTest extends GroovyTestCase {

    static final TreeAdaptor adaptor = new CommonTreeAdaptor(){
        @Override
        Object create(Token payload) {
            return new CommonTree(payload)
        }
    }

    GroovyParserTest() {
    }

    @Override
    protected void setUp() {
    }

    public void testSimpleExpressionTwoNumbersAdd() {
        testTwoArgumentExpression("+", "10", "5")
    }

    public void testSimpleExpressionTwoNumbersMinus() {
        testTwoArgumentExpression("-", "11", "6")
    }

    public void testSimpleExpressionTwoNumbersMult() {
        testTwoArgumentExpression("*", "12", "7")
    }

    public void testSimpleExpressionTwoNumbersDiv() {
        testTwoArgumentExpression("/", "12", "8")
    }

    public void testTwoArgumentExpression(String op, String oper1, String oper2) {
        def code = """
        ${oper1} ${op} ${oper2}
        """

        def ast = parse(code)
        assertNotNull("Was not able to parse following code: " + code, ast)

        assertEquals(op, ast.getToken().getText())
        assertEquals(2, ast.childCount)
        assertEquals(oper1, ast.getChild(0).getText())
        assertEquals(oper2, ast.getChild(1).getText())
    }


    private CommonTree parse(String inputString) throws RecognitionException {
        def inputStream = new ANTLRStringStream(inputString);
        def lexer = new RppLexer(inputStream);
        def tokenStream = new CommonTokenStream(lexer);
        def parser = new RppParser(tokenStream);
        parser.setTreeAdaptor(adaptor)
        return (CommonTree) parser.prog().getTree();
    }
}
