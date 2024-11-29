#include "core/Mutator.h"

void Mutator::SEL_operator(std::string query, MutationData &mutationData, TreeNode *mutantTreeNode)
{
    mutantTreeNode->AddMutantChildren(query);
    std::string lower_query = query;
    std::transform(lower_query.begin(), lower_query.end(), lower_query.begin(), ::tolower);

    size_t dis_pos = lower_query.find("distinct");
    if (dis_pos != std::string::npos)
    {
        // distinct clause exists
        query.erase(dis_pos, 9); // Length of "DISTINCT" is 8 but erase 9 to remove an extra space too
        mutantTreeNode->AddMutantChildren(query);
        mutationData.mutated_queries.push_back(query);
    }
    else
    {
        size_t select_pos = lower_query.find("select");
        if (select_pos != std::string::npos)
        {
            query.insert(select_pos + 6, " DISTINCT");
            mutantTreeNode->AddMutantChildren(query);
            mutationData.mutated_queries.push_back(query);
        }
        else
        {
            std::cerr << "Error: 'SELECT' keyword not found in query." << std::endl;
        }
    }
    return;
}

void Mutator::JOI_operator(std::string query, MutationData &mutationData, TreeNode *mutantTree)
{
    /**
     * LEFT JOIN
     * RIGHT JOIN
     * INNER JOIN
     * FULL JOIN
     * SEMI JOIN
     * ANTI JOIN
     */

    std::string lower_query = query;
    std::transform(lower_query.begin(), lower_query.end(), lower_query.begin(), ::tolower);
    size_t join_pos_exist = lower_query.find(" join ");
    if (join_pos_exist != std::string::npos)
    {
        // std::cout << "JOIN CLAUSE EXISTS: " << query << std::endl;
        // join clause exists
        mutantTree->AddMutantChildren(query);
        const std::string join_types[] = {"left", "right", "full", "semi", "anti"};
        size_t join_pos = lower_query.find("join");

        for (size_t i = 0; i < join_types->size(); i++)
        {
            const std::string &type = join_types[i];
            size_t type_pos = query.rfind(type, join_pos - 1); // Search before "join"
            if (type_pos != std::string::npos)
            {
                // Ensure the found type is immediately before "join"
                if (type_pos + type.length() + 1 == join_pos)
                { // +1 for the space
                    auto temp_type = type;
                    for (const auto &type_mutant : join_types)
                    {
                        if (type.compare(type_mutant) != 0)
                        {
                            query.erase(type_pos, temp_type.size());
                            query.insert(type_pos, type_mutant);
                            mutationData.mutated_queries.push_back(query);
                            temp_type = type_mutant;
                        }
                    }
                }
                break;
            }
        }
    }
    else
    {
        // std::cout << "JOIN CLAUSE DOES NOT EXISTS: " << query << std::endl;

        // no mutation if join clause didn't exist
        return;
    }
}

void Mutator::InternalMutate(std::string &query, MutationData &mutationData, TreeNode *mutantTreeNode, MutationOperator operatr)
{
    switch (operatr)
    {
    case MutationOperator::SELECT:
    {
        // std::cout << "Before select mutation for query: " << query << std::endl;
        SEL_operator(query, mutationData, mutantTreeNode);
        // std::cout << "After select mutation for query: " << query << std::endl;
        for (auto childMutantTreeNode : mutantTreeNode->children)
        {
            InternalMutate(childMutantTreeNode->query, mutationData, childMutantTreeNode, MutationOperator::JOIN);
        }

        break;
    }
    case MutationOperator::JOIN:
    {
        JOI_operator(query, mutationData, mutantTreeNode);
        // std::cout << "After Join Mutation for: " << query << std::endl;
        for (auto childMutantTreeNode : mutantTreeNode->children)
        {
            InternalMutate(childMutantTreeNode->query, mutationData, childMutantTreeNode, MutationOperator::RELATIONAL);
        }
        // std::cout << "Internal Mutation Completed for JOIN" << std::endl;
        break;
    }
    default:
    {
        // std::cout << "Internal Mutation Completed" << std::endl;
        break;
    }
    }
}

void Mutator::Mutate(MutationData &mutationData, TreeNode *mutantTreeNode)
{
    if (mutantTreeNode == nullptr)
    {
        auto mutantTree = new MutantTree(mutationData.original_query);

        string query = mutationData.original_query;
        mutationData.mutated_queries.push_back(query);
        // std::cout << "ASS" << std::endl;
        InternalMutate(query, mutationData, mutantTree->root, MutationOperator::SELECT);
        // std::cout << "ASS" << std::endl;
    }
}
