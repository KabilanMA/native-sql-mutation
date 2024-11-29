#include "core/TreeNode.h"

TreeNode::TreeNode(const string &query) : query(query)
{
}

TreeNode::~TreeNode()
{
    for (auto node : children)
    {
        node->~TreeNode();
    }
    delete this;
}

void TreeNode::AddMutantChildren(const string &mutant_query)
{
    auto a = new TreeNode(mutant_query);
    children.push_back(a);
}
