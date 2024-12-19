#include "core/Mutator.h"
#include <cmath>

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

int countKeyword(const std::string &query, std::string keyword)
{
    size_t pos = 0;
    int count = 0;
    // std::string keyword = "AND";

    while ((pos = query.find(keyword, pos)) != std::string::npos)
    {
        bool is_start_valid = (pos == 0 || !std::isalnum(query[pos - 1]));
        bool is_end_valid = (pos + keyword.length() == query.length() || !std::isalnum(query[pos + keyword.length()]));
        if (is_start_valid && is_end_valid)
        {
            count++;
        }
        pos += keyword.length();
    }
    return count;
}

void addWhereClauseMutants(std::string query, size_t start_pos, size_t end_pos, MutationData &mutationData, TreeNode *mutantTreeNode)
{
    const std::string mutant_operators[] = {"<", "<=", "=", ">", ">=", "<>"};
    std::string clause = query.substr(start_pos, end_pos - start_pos);

    for (const auto &mutant_operator : mutant_operators)
    {
        size_t operator_pos = clause.find(mutant_operator);
        if (operator_pos != std::string::npos)
        {
            for (const auto &replacement_operator : mutant_operators)
            {
                if (mutant_operator != replacement_operator)
                {
                    std::string mutated_query = query;
                    mutated_query.replace(start_pos + operator_pos, mutant_operator.length(), replacement_operator);

                    mutantTreeNode->AddMutantChildren(mutated_query);
                    mutationData.mutated_queries.push_back(mutated_query);
                }
            }
            break;
        }
    }
}

void ROR_OperatorComplexHelper(std::string query, MutationData &mutationData, TreeNode *mutantTreeNode, size_t start_pos, size_t end_pos)
{
    const std::string mutant_operators[] = {"<", "<=", "=", ">", ">=", "<>"};
    std::string lower_query = query;
    std::transform(lower_query.begin(), lower_query.end(), lower_query.begin(), ::tolower);

    size_t a = lower_query.rfind(" and ", end_pos - 1);
    size_t b = lower_query.rfind(" or ", end_pos - 1);

    if (a == std::string::npos)
    {
        if (b == std::string::npos)
        {
            std::string clause = query.substr(start_pos, end_pos - start_pos);

            for (const auto &mutant_operator : mutant_operators)
            {
                size_t operator_pos = clause.find(mutant_operator);
                if (operator_pos != std::string::npos)
                {
                    for (const auto &replacement_operator : mutant_operators)
                    {
                        if (mutant_operator != replacement_operator)
                        {
                            std::string mutated_query = query;
                            mutated_query.replace(start_pos + operator_pos, mutant_operator.length(), replacement_operator);

                            mutantTreeNode->AddMutantChildren(mutated_query);
                            mutationData.mutated_queries.push_back(mutated_query);
                        }
                    }
                    break;
                }
            }
        }
        else
        {
            ROR_OperatorComplexHelper(query, mutationData, mutantTreeNode, start_pos, b);

            size_t end_pos_temp = std::min(lower_query.find(" ", b + 4), lower_query.find(";", b + 4));

            std::string clause = query.substr(b + 4, end_pos_temp - (b + 4));

            for (const auto &mutant_operator : mutant_operators)
            {
                size_t operator_pos = clause.find(mutant_operator);
                if (operator_pos != std::string::npos)
                {
                    for (const auto &replacement_operator : mutant_operators)
                    {
                        if (mutant_operator != replacement_operator)
                        {
                            std::string mutated_query = query;
                            mutated_query.replace(b + 4 + operator_pos, mutant_operator.length(), replacement_operator);

                            mutantTreeNode->AddMutantChildren(mutated_query);
                            mutationData.mutated_queries.push_back(mutated_query);
                            // cout << mutated_query << endl;
                            ROR_OperatorComplexHelper(mutated_query, mutationData, mutantTreeNode, start_pos, b);
                        }
                    }
                    break;
                }
            }
        }
    }
    else
    {
        if (b == std::string::npos)
        {
            ROR_OperatorComplexHelper(query, mutationData, mutantTreeNode, start_pos, a);

            size_t end_pos_temp = std::min(lower_query.find(" ", a + 5), lower_query.find(";", a + 5));

            std::string clause = query.substr(a + 5, end_pos_temp - (a + 5));

            for (const auto &mutant_operator : mutant_operators)
            {
                size_t operator_pos = clause.find(mutant_operator);
                if (operator_pos != std::string::npos)
                {
                    for (const auto &replacement_operator : mutant_operators)
                    {
                        if (mutant_operator != replacement_operator)
                        {
                            std::string mutated_query = query;
                            mutated_query.replace(a + 5 + operator_pos, mutant_operator.length(), replacement_operator);

                            mutantTreeNode->AddMutantChildren(mutated_query);
                            mutationData.mutated_queries.push_back(mutated_query);
                            // cout << mutated_query << endl;
                            ROR_OperatorComplexHelper(mutated_query, mutationData, mutantTreeNode, start_pos, a);
                        }
                    }
                    break;
                }
            }
        }
        else
        {
            size_t capped_pos = std::min(a, b);
            size_t size = 4;
            if (a == capped_pos)
            {
                size = 5;
            }
            ROR_OperatorComplexHelper(query, mutationData, mutantTreeNode, start_pos, capped_pos);

            size_t end_pos_temp = std::min(lower_query.find(" ", capped_pos + size), lower_query.find(";", capped_pos + size));

            std::string clause = query.substr(capped_pos + size, end_pos_temp - (capped_pos + size));

            for (const auto &mutant_operator : mutant_operators)
            {
                size_t operator_pos = clause.find(mutant_operator);
                if (operator_pos != std::string::npos)
                {
                    for (const auto &replacement_operator : mutant_operators)
                    {
                        if (mutant_operator != replacement_operator)
                        {
                            std::string mutated_query = query;
                            mutated_query.replace(capped_pos + size + operator_pos, mutant_operator.length(), replacement_operator);

                            mutantTreeNode->AddMutantChildren(mutated_query);
                            mutationData.mutated_queries.push_back(mutated_query);
                            // cout << mutated_query << endl;
                            ROR_OperatorComplexHelper(mutated_query, mutationData, mutantTreeNode, start_pos, capped_pos);
                        }
                    }
                    break;
                }
            }
        }
    }
}

std::vector<std::string> splitOperators(const std::string &input)
{
    std::vector<std::string> result;
    std::istringstream stream(input);
    std::string word;
    while (stream >> word)
    {
        result.push_back(word);
    }
    return result;
}

void generatePermutationOfConjunction(int and_count, int or_count, std::string value, std::vector<std::vector<std::string>> &permutations)
{
    if (and_count == 0 || or_count == 0)
    {
        std::vector<std::string> result;
        std::istringstream stream(value);
        std::string word;
        while (stream >> word)
        {
            result.push_back(word);
        }
        permutations.push_back(result);
        return;
    }

    if (and_count > 0)
    {
        generatePermutationOfConjunction(and_count - 1, or_count, value + (value.empty() ? "" : " ") + "AND", permutations);
    }

    if (or_count > 0)
    {
        generatePermutationOfConjunction(and_count - 1, or_count, value + (value.empty() ? "" : " ") + "OR", permutations);
    }
}

void Mutator::ROR_OperatorComplex(std::string query, MutationData &mutationData, TreeNode *mutantTreeNode, std::string clause)
{
    std::string lower_query = query;
    std::transform(lower_query.begin(), lower_query.end(), lower_query.begin(), ::tolower);
    size_t where_pos = lower_query.find(clause);
    size_t conjective_operator_count = countKeyword(lower_query, "and");
    conjective_operator_count += countKeyword(lower_query, "or");
    if (where_pos == std::string::npos || conjective_operator_count == 0)
        return;
    // cout << conjective_operator_count << endl;
    // cout << lower_query << endl;
    const std::string mutant_operators[] = {"<", "<=", "=", ">", ">=", "<>"};

    size_t search_start_pos = where_pos + clause.size();

    size_t last_and_pos = lower_query.rfind(" and ");
    last_and_pos = last_and_pos == std::string::npos ? -2 : last_and_pos;
    size_t last_or_pos = lower_query.rfind(" or ");
    last_or_pos = last_or_pos == std::string::npos ? -1 : last_or_pos;
    size_t last_conj_pos = std::max(last_and_pos + 5, last_or_pos + 4);
    size_t end_pos = lower_query.find(" ", last_conj_pos);
    end_pos = end_pos == std::string::npos ? lower_query.find(";", last_conj_pos) : end_pos;
    // size_t last_conj_pos = std::max(lower_query.rfind(" and ") + 5, lower_query.rfind(" or ") + 4);
    // size_t end_pos = lower_query.find(" ", std::max(lower_query.rfind(" and ") + 5, lower_query.rfind(" or ") + 4));

    std::vector<std::vector<std::string>> conjective_operator_permutations = {};
    generatePermutationOfConjunction(conjective_operator_count, conjective_operator_count, {}, conjective_operator_permutations);

    for (size_t j = 0; j < conjective_operator_permutations.size(); j++)
    {
        // conjective_operator_count;
        size_t count = 0;
        std::string mutated_query = query;
        lower_query = mutated_query;
        std::transform(lower_query.begin(), lower_query.end(), lower_query.begin(), ::tolower);

        for (size_t i = search_start_pos; i < end_pos - 3; i++)
        {
            if (lower_query[i] == 'a')
            {
                if (lower_query[i + 1] == 'n' && lower_query[i + 2] == 'd' && lower_query[i + 3] == ' ' && lower_query[i - 1] == ' ')
                {
                    mutated_query.replace(i, 3, conjective_operator_permutations[j][count++]);
                    lower_query = mutated_query;
                    std::transform(lower_query.begin(), lower_query.end(), lower_query.begin(), ::tolower);
                }
            }
            else if (lower_query[i] == 'o')
            {
                if (lower_query[i + 1] == 'r' && lower_query[i + 2] == ' ' && lower_query[i - 1] == ' ')
                {
                    mutated_query.replace(i, 2, conjective_operator_permutations[j][count++]);
                    lower_query = mutated_query;
                    std::transform(lower_query.begin(), lower_query.end(), lower_query.begin(), ::tolower);
                }
            }
        }
        ROR_OperatorComplexHelper(mutated_query, mutationData, mutantTreeNode, search_start_pos, end_pos);
    }
}

void Mutator::ROR_Operator(std::string query, MutationData &mutationData, TreeNode *mutantTreeNode)
{
    /**
     * @brief Mutate the relational operator (<,<=,=,>,>=,<>) with each other
     *
     */

    const std::string mutant_operators[] = {"<", "<=", "=", ">", ">=", "<>"};

    std::string lower_query = query;

    std::transform(lower_query.begin(), lower_query.end(), lower_query.begin(), ::tolower);

    size_t where_pos = lower_query.find(" where ");
    size_t having_pos = lower_query.find(" having ");

    if (where_pos != std::string::npos || having_pos != std::string::npos)
    {
        size_t and_pos = lower_query.find(" and ");
        size_t or_pos = lower_query.find(" or ");
        if (and_pos == std::string::npos && or_pos == std::string::npos)
        {
            size_t clause_start = (where_pos != std::string::npos) ? where_pos : having_pos;
            std::string clause = query.substr(clause_start);

            for (const auto &mutant_operator : mutant_operators)
            {
                size_t operator_pos = clause.find(mutant_operator);
                if (operator_pos != std::string::npos)
                {
                    for (const auto &replacement_operator : mutant_operators)
                    {
                        if (mutant_operator != replacement_operator)
                        {
                            std::string mutated_query = query;
                            mutated_query.replace(clause_start + operator_pos, mutant_operator.length(), replacement_operator);

                            mutantTreeNode->AddMutantChildren(mutated_query);
                            mutationData.mutated_queries.push_back(mutated_query);
                        }
                    }
                    break;
                }
            }
        }
        else
        {
            if (where_pos != std::string::npos)
                ROR_OperatorComplex(query, mutationData, mutantTreeNode, " where ");

            if (having_pos != std::string::npos)
                ROR_OperatorComplex(query, mutationData, mutantTreeNode, " having ");
        }
    }
    return;
}

size_t findClause(const std::string &query, const std::string &clause)
{
    size_t pos = query.find(clause);
    return (pos != std::string::npos) ? pos : query.length();
}

static std::string excludeClauses(const std::string &query, size_t group_by_pos, size_t order_by_pos)
{
    size_t exclude_start = std::min(group_by_pos, order_by_pos);
    return query.substr(0, exclude_start);
}

std::string excludeExistsSelects(const std::string &query, const std::regex &exists_regex)
{
    return std::regex_replace(query, exists_regex, ""); // Remove EXISTS subquery portions
}

std::string mutateStringWithNew(const std::string &query, size_t pos, const std::string &old_value, const std::string &new_value)
{
    std::string mutated_query = query;
    mutated_query.replace(pos, old_value.length(), new_value);
    return mutated_query;
}

void Mutator::UOI_Operator(std::string query, MutationData &mutationData, TreeNode *mutantTreeNode)
{
    /**
     * @brief Each arithmetic expression or reference to a number e is replaced by -e, e + 1 and e - 1. References to numbers are not mutated either inside of GROUP BY and ORDER BY clauses or in the select-list of an EXISTS subquery.
     *
     */

    std::string processed_query = query;
    std::transform(processed_query.begin(), processed_query.end(), processed_query.begin(), ::tolower);

    std::regex number_regex(R"((\b\d+\b))");
    std::smatch match;

    while (std::regex_search(processed_query, match, number_regex))
    {
        std::string number = match.str(0);
        size_t number_pos = match.position(0);

        std::string mutation_neg = mutateStringWithNew(processed_query, number_pos, number, "-" + number);
        mutantTreeNode->AddMutantChildren(mutation_neg);
        mutationData.mutated_queries.push_back(mutation_neg);

        std::string mutation_add = mutateStringWithNew(processed_query, number_pos, number, number + "+1");
        mutantTreeNode->AddMutantChildren(mutation_add);
        mutationData.mutated_queries.push_back(mutation_add);

        std::string mutation_sub = mutateStringWithNew(processed_query, number_pos, number, number + "-1");
        mutantTreeNode->AddMutantChildren(mutation_sub);
        mutationData.mutated_queries.push_back(mutation_sub);

        processed_query = match.suffix().str();
    }
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
            size_t type_pos = lower_query.rfind(type, join_pos - 1); // Search before "join"
            if (type_pos != std::string::npos)
            {
                // cout << "join exists" << endl;
                // Ensure the found type is immediately before "join"
                if (type_pos + type.length() + 1 == join_pos)
                { // +1 for the space
                    auto temp_type = type;
                    for (const auto &type_mutant : join_types)
                    {
                        // cout << type_mutant << endl;
                        if (type.compare(type_mutant) != 0)
                        {
                            query.erase(type_pos, temp_type.size());
                            query.insert(type_pos, type_mutant);
                            mutantTree->AddMutantChildren(query);
                            // cout << query << endl;
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
        // std::cout << "Select Mutant Count: " << mutantTreeNode->children.size() << std::endl;
        for (auto childMutantTreeNode : mutantTreeNode->children)
        {
            // std::cout << "Mutant Select: " << childMutantTreeNode->query << std::endl;
            InternalMutate(childMutantTreeNode->query, mutationData, childMutantTreeNode, MutationOperator::JOIN);
        }
        break;
    }
    case MutationOperator::JOIN:
    {
        JOI_operator(query, mutationData, mutantTreeNode);
        // std::cout << "After Join Mutation for: " << query << std::endl;
        // std::cout << "Join Mutant Count: " << mutantTreeNode->children.size() << std::endl;
        for (auto childMutantTreeNode : mutantTreeNode->children)
        {
            // std::cout << "Mutant Join: " << childMutantTreeNode->query << std::endl;
            InternalMutate(childMutantTreeNode->query, mutationData, childMutantTreeNode, MutationOperator::RELATIONAL_OR);
        }
        // std::cout << "Internal Mutation Completed for JOIN" << std::endl;
        break;
    }
    case MutationOperator::RELATIONAL_OR:
    {
        ROR_Operator(query, mutationData, mutantTreeNode);
        // std::cout << "ROR Mutant Count: " << mutantTreeNode->children.size() << std::endl;
        for (auto childMutantTreeNode : mutantTreeNode->children)
        {
            // std::cout << "Mutant ROR: " << childMutantTreeNode->query << std::endl;
            InternalMutate(childMutantTreeNode->query, mutationData, childMutantTreeNode, MutationOperator::UNARY_IOR);
        }
        break;
    }
    case MutationOperator::UNARY_IOR:
    {
        UOI_Operator(query, mutationData, mutantTreeNode);
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
