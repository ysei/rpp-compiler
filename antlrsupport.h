#ifndef _ANTLRSUPPORT_H__
#define _ANTLRSUPPORT_H__

#include <string>
#include <antlr3basetree.h>
#include <antlr3commontoken.h>

inline pANTLR3_COMMON_TOKEN getToken(pANTLR3_BASE_TREE node)
{
    return node->getToken(node);
}

inline unsigned char * getTokenText(pANTLR3_COMMON_TOKEN token)
{
    return token->getText(token)->chars;
}

inline unsigned char * getNodeText(pANTLR3_BASE_TREE node)
{
    return getTokenText(getToken(node));
}

inline std::string getNodeString(pANTLR3_BASE_TREE node)
{
    return std::string((char *)getNodeText(node));
}

inline unsigned int getTokenType(pANTLR3_COMMON_TOKEN token)
{
    return token->type;
}

inline unsigned int getTokenType(pANTLR3_BASE_TREE node)
{
    return getTokenType(getToken(node));
}

inline pANTLR3_BASE_TREE getNodeChild(pANTLR3_BASE_TREE node, unsigned int index)
{
    return (pANTLR3_BASE_TREE) node->getChild(node, index);
}

inline unsigned int getNodeChildCount(pANTLR3_BASE_TREE node)
{
    return node->getChildCount(node);
}

#endif
