#include "alias.h"

class TreeNode
{
public:
    vector<TreeNode *> children = {};
    string query;

public:
    TreeNode(string query);
    ~TreeNode();

    void AddMutantChildren(string mutant_query);
};