package org.dubikdev.toycompiler

class ParserWhileTest extends ParserBaseTestCase {
    public void testEmptyWhileLoop() {
        def code = """
        def func(x)
            while x > 0
            end
        end
        """

        parseAndAssertIfError(code)
    }

    public void testWhileLoop() {
        def code = """
        def func(x)
            while x > 0
                x = x - 1
            end
        end
        """

        parseAndAssertIfError(code)
    }

    public void testBigWhileLoop() {
        def code = """
        def func(x)
            while x > 1 && x < 100
                x = x + i
                k = k + 1
            end
        end
        """

        parseAndAssertIfError(code)
    }
}
