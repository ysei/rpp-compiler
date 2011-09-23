package org.dubikdev.toycompiler

class ParserFuncTest extends ParserBaseTestCase {
    public void testEmptyFunctionDeclNoParams() {
        def code = """
        def func
        end
        """

        parseAndAssertIfError(code)
    }

    public void testEmptyFunctionDeclWithOneParam() {
        def code = """
        def func(x)
        end
        """

        parseAndAssertIfError(code)
    }

    public void testEmptyFunctionDeclWithTwoParams() {
        def code = """
        def func(x, y)
        end
        """

        parseAndAssertIfError(code)
    }

    public void testEmptyFunctionDeclWithTwoParamsAndExpression() {
        def code = """
        def func(x, y, 1 + z)
        end
        """

        parseAndAssertIfError(code)
    }
}
