package org.dubikdev.toycompiler;

import org.antlr.runtime.ANTLRStringStream
import org.antlr.runtime.CommonTokenStream
import org.antlr.runtime.RecognitionException
import org.antlr.runtime.tree.CommonTree

import org.antlr.runtime.Token

import org.antlr.runtime.tree.CommonTreeAdaptor
import org.antlr.runtime.tree.TreeAdaptor
import org.antlr.runtime.tree.CommonErrorNode

class ParserTernaryOperatorsTest extends ParserBaseTestCase {

    public void testSimpleExpressionTwoNumbersAdd() {
        testTernaryOperator("+", "10", "5")
    }

    public void testSimpleExpressionTwoNumbersMinus() {
        testTernaryOperator("-", "11", "6")
    }

    public void testSimpleExpressionTwoNumbersMult() {
        testTernaryOperator("*", "12", "7")
    }

    public void testSimpleExpressionTwoNumbersDiv() {
        testTernaryOperator("/", "12", "8")
    }

    public void testSimpleExpressionTwoNumbersRightShift() {
        testTernaryOperator(">>", "1", "8")
    }

    public void testSimpleExpressionTwoNumbersLeftShift() {
        testTernaryOperator("<<", "256", "4")
    }

    public void testAssignVarAndNumber() {
        testTernaryOperator("=", "x", "8")
    }

    public void testTwoVarAnd() {
        testTernaryOperator("&&", "x", "y")
    }

    public void testTwoVarOr() {
        testTernaryOperator("||", "x", "y")
    }

    public void testTwoVarLess() {
        testTernaryOperator("<", "x", "y")
    }

    public void testTwoVarGreater() {
        testTernaryOperator(">", "x", "y")
    }


    public void testTwoVarLessEqual() {
        testTernaryOperator("<=", "x", "y")
    }

    public void testTwoVarGreateEqualr() {
        testTernaryOperator(">=", "x", "y")
    }

    public void testTwoVarEqual() {
        testTernaryOperator("==", "x", "y")
    }

    public void testTwoVarNotEqual() {
        testTernaryOperator("!=", "x", "y")
    }

    public void testBitOr() {
        testTernaryOperator("|", "256", "1")
    }

    public void testBitAnd() {
        testTernaryOperator("&", "256", "1")
    }

    public void testBitXor() {
        testTernaryOperator("^", "256", "1")
    }

    public void testParenthisisExpr() {
        def code = """
        10.4 + 4 * (x - 1)
        """

        def ast = parse(code)
        assertNotNull("Was not able to parse following code: " + code, ast)

        assertEquals("+", ast.getText())
        assertEquals(2, ast.childCount)
        assertEquals("10.4", ast.getChild(0).getText())
        assertEquals("*", ast.getChild(1).getText())
        assertEquals(2, ast.getChild(1).childCount)  // 4 * (x - 1)
        assertEquals("4", ast.getChild(1).getChild(0).getText())
        assertEquals("-", ast.getChild(1).getChild(1).getText())
    }

    public void testTernaryOperator(String op, String oper1, String oper2) {
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
}
