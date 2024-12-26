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
    LKE_OP
};

class Mutator
{
public:
    void Mutate(MutationData &mutationData, TreeNode *mutantTreeNode = nullptr);

    void SEL_operator(std::string query, MutationData &mutationData, TreeNode *mutantTreeNode);
    void JOI_operator(std::string query, MutationData &mutationData, TreeNode *mutantTreeNode);
    void ROR_Operator(std::string query, MutationData &mutationData, TreeNode *mutantTreeNode);
    void UOI_Operator(std::string query, MutationData &mutationData, TreeNode *mutantTreeNode);
    void AOR_Operator(std::string query, MutationData &mutationData, TreeNode *mutantTreeNode);
    void BTW_Operator(std::string query, MutationData &mutationData, TreeNode *mutantTree);

private:
    void InternalMutate(std::string &query, MutationData &mutationData, TreeNode *mutantTree, MutationOperator operatr);
    void ROR_OperatorComplex(std::string query, MutationData &mutationData, TreeNode *mutantTreeNode, std::string clause);
};
