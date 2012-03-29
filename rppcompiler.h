#ifndef _RPPCOMPILER_H__
#define _RPPCOMPILER_H__

class ASTNode;

class RppCompiler
{
public:
    RppCompiler();
    virtual ~RppCompiler();

    void compile(const char * fileName);

    ASTNode * rootAstNode() const;

private:
    ASTNode * m_root;
};

#endif // _RPPCOMPILER_H__
