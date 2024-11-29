#include "alias.h"

class TreeNode
{
public:
    vector<TreeNode *> children;
    string query;

public:
    TreeNode(const string &query);
    ~TreeNode();

    void AddMutantChildren(const string &mutant_query);
};