#include "core/TreeNode.h"

class MutantTree
{
public:
    TreeNode *root;

public:
    MutantTree(const string &original_query);
    ~MutantTree();

    void PrintMutantTree();
};