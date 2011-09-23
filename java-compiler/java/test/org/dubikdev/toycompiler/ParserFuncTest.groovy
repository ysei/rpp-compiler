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

    public void testFunctionWithAssignment() {
        def code = """
        def func(x)
        y = x
        end
        """

        parseAndAssertIfError(code)
    }

    public void testFunctionWithAssignmentAndExpression() {
        def code = """
        def func(x)
        y = (x + 10) * 3
        end
        """

        parseAndAssertIfError(code)
    }

    public void testFunctionWithAssignmentAndExpressionAndReturn() {
        def code = """
        def func(x)
        y = (x + 10) * 3
        return y
        end
        """

        parseAndAssertIfError(code)
    }

    public void testFuncIfCond() {
        def code = """
        def func(x)
            if x == 0
                f()
            else
                z()
            end
        end
        """

        parseAndAssertIfError(code)
    }

    public void testFuncIfEmpty() {
        def code = """
        def func(x)
            if x == 0
            end
        end
        """
        parseAndAssertIfError(code)
    }

    public void testFuncIfElseEmpty() {
        def code = """
        def func(x)
            if x == 0
            else
            end
        end
        """
        parseAndAssertIfError(code)
    }

    public void testFuncIfCondError() {
        def code = """
        def func(x)
            if x == 0
                fn fn
            end
        end
        """
        parseAndAssertIfNotError(code)
    }
}
