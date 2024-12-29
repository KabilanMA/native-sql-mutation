#pragma once
#include "MutantTree.h"
#include <algorithm>
#include <regex>

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
    RELATIONAL_OR, // relatinal operator
    LOGICAL_CR,    // logical connector operator
    UNARY_IOR,
    LOGICAL_AOR,
    BTW_OP,
    LKE_OP,
    NLF_PRE,
    AGR_OP
};

class Mutator
{
public:
    void Mutate(MutationData &mutationData, TreeNode *mutantTreeNode = nullptr);

    void SEL_operator(string query, MutationData &mutationData, TreeNode *mutantTreeNode);
    void JOI_operator(string query, MutationData &mutationData, TreeNode *mutantTreeNode);
    void ROR_Operator(string query, MutationData &mutationData, TreeNode *mutantTreeNode);
    void UOI_Operator(string query, MutationData &mutationData, TreeNode *mutantTreeNode);
    void AOR_Operator(string query, MutationData &mutationData, TreeNode *mutantTreeNode);
    void BTW_Operator(string query, MutationData &mutationData, TreeNode *mutantTreeNode);
    void LKE_Operator(string query, MutationData &mutationData, TreeNode *mutantTreeNode, int type = 1);
    void NFL_Operator(string query, MutationData &mutationData, TreeNode *mutantTreeNode);
    void AGR_Operator(string query, MutationData &mutationData, TreeNode *mutantTreeNode, size_t start_pos = 0);

private:
    void InternalMutate(string &query, MutationData &mutationData, TreeNode *mutantTree, MutationOperator operatr);
    void ROR_OperatorComplex(string query, MutationData &mutationData, TreeNode *mutantTreeNode, string clause);
};
