#pragma once
#include "MutantTree.h"
#include <algorithm>

struct MutationData
{
    MutationData(string query) : original_query(query) {}

    string original_query;
    vector<string> mutated_queries;
    vector<string> equivalent_queries;
    // unique_ptr<QueryResult> original_result = nullptr;
};

enum MutationOperator
{
    SELECT,
    JOIN,
    RELATIONAL
};

class Mutator
{
public:
    void Mutate(MutationData &mutationData, TreeNode *mutantTreeNode = nullptr);

    void SEL_operator(std::string query, MutationData &mutationData, TreeNode *mutantTreeNode);
    void JOI_operator(std::string query, MutationData &mutationData, TreeNode *mutantTreeNode);

private:
    void InternalMutate(std::string &query, MutationData &mutationData, TreeNode *mutantTree, MutationOperator operatr);
};
