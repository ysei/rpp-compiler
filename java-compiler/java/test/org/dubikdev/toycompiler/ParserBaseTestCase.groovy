package org.dubikdev.toycompiler

import org.antlr.runtime.tree.CommonTree
import org.antlr.runtime.tree.CommonTreeAdaptor
import org.antlr.runtime.tree.TreeAdaptor
import org.antlr.runtime.RecognitionException
import org.antlr.runtime.ANTLRStringStream
import org.antlr.runtime.CommonTokenStream
import org.antlr.runtime.Token

class ParserBaseTestCase extends GroovyTestCase {
    static final TreeAdaptor adaptor = new CommonTreeAdaptor() {
        @Override
        Object create(Token payload) {
            return new CommonTree(payload)
        }
    }

    public void testDummy() {

    }

    CommonTree parse(String inputString) throws RecognitionException {
        RppParser parser = createParser(inputString)
        return (CommonTree) parser.prog().getTree();
    }

    private RppParser createParser(String inputString) {
        def inputStream = new ANTLRStringStream(inputString);
        def lexer = new RppLexer(inputStream);
        def tokenStream = new CommonTokenStream(lexer);
        def parser = new RppParser(tokenStream);
        parser.setTreeAdaptor(adaptor)
        return parser
    }

    void parseAndAssertIfError(String code) {
        def parser = createParser(code)
        parser.setTreeAdaptor(adaptor)
        def ast = parser.prog();
        assertEquals("Wasn't be able to parse: " + code, 0, parser.getNumberOfSyntaxErrors())
    }
}
