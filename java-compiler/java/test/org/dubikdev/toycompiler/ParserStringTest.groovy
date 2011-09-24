package org.dubikdev.toycompiler

class ParserStringTest extends ParserBaseTestCase {
    public void testStringToken() {
        def code = """
        x = "Hello"
        """

        def ast = parse(code)

        assertEquals('=', ast.getText())
        assertEquals(2, ast.getChildCount())
        assertEquals("\"Hello\"", ast.getChild(1).getText())
        assertEquals(RppLexer.STRING, ast.getChild(1).getType())
    }

    public void testSimpleString() {
        def code = """
        def func
            x = "Hello"
        end
        """

        parseAndAssertIfError(code)
    }

    public void testSingleQuoteString() {
        def code = """
        def func
            x = 'Hello'
        end
        """

        parseAndAssertIfError(code)
    }
}
