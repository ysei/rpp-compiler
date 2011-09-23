package org.dubikdev.toycompiler

import org.antlr.runtime.Token

class ParserNewLineTest extends ParserBaseTestCase {
    public void testOneLineExpression() {
        def code = "x"
        testUnaryOperator(code, "x")
    }

    public void testOneLineExpressionCRInTheEnd() {
        def code = "x\r"
        testUnaryOperator(code, "x")
    }

    public void testOneLineExpressionCRInBeggining() {
        def code = "\rx"
        testUnaryOperator(code, "x")
    }

    public void testOneLineExpressionCRInBoth() {
        def code = "\rx\r"
        testUnaryOperator(code, "x")
    }

    private def testUnaryOperator(String code, String op) {
        def ast = parse(code)
        assertNotNull("Was not able to parse following code: " + code, ast)
        assertFalse("Can't parse: " + ast.toString(), ast.getType() == Token.INVALID_TOKEN_TYPE)
        assertEquals(op, ast.getText())
    }
}
