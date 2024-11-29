#include "core/MutantTree.h"

MutantTree::MutantTree(const string &original_query)
{
    root = new TreeNode(original_query);
}

void MutantTree::PrintMutantTree()
{
}

MutantTree::~MutantTree()
{
    delete root;
    delete this;
}
