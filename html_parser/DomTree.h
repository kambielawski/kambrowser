#include <iostream>

enum DomNodeType
{
    content,
    open_tag,
    close_tag
};

struct Attribute
{
    std::string attr_name;
    std::string value;
};

struct DomNode 
{
    DomNodeType dn_type; 
    Attribute *dn_attributes;  /* attributes of HTML tag */
    DomNode **dn_children;     /* array of DomNodes */
};

#ifndef DOM_TREE_H
#define DOM_TREE_H

class DomTree {
private:
DomNode *dom_root;

public:
DomTree();
~DomTree();

};

#endif
