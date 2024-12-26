#include "core/Mutator.h"
#include <cmath>

void Mutator::SEL_operator(string query, MutationData &mutationData, TreeNode *mutantTreeNode)
{
    mutantTreeNode->AddMutantChildren(query);
    string lower_query = query;
    std::transform(lower_query.begin(), lower_query.end(), lower_query.begin(), ::tolower);

    size_t dis_pos = lower_query.find("distinct");
    if (dis_pos != string::npos)
    {
        // distinct clause exists
        query.erase(dis_pos, 9); // Length of "DISTINCT" is 8 but erase 9 to remove an extra space too
        mutantTreeNode->AddMutantChildren(query);
        mutationData.mutated_queries.push_back(query);
    }
    else
    {
        size_t select_pos = lower_query.find("select");
        if (select_pos != string::npos)
        {
            query.insert(select_pos + 6, " DISTINCT");
            mutantTreeNode->AddMutantChildren(query);
            mutationData.mutated_queries.push_back(query);
        }
        else
        {
            std::cerr << "Error: 'SELECT' keyword not found in query." << endl;
        }
    }
    return;
}

int countKeyword(const string &query, string keyword)
{
    size_t pos = 0;
    int count = 0;
    // std::string keyword = "AND";

    while ((pos = query.find(keyword, pos)) != string::npos)
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

void addWhereClauseMutants(string query, size_t start_pos, size_t end_pos, MutationData &mutationData, TreeNode *mutantTreeNode)
{
    const string mutant_operators[] = {"<", "<=", "=", ">", ">=", "<>"};
    string clause = query.substr(start_pos, end_pos - start_pos);

    for (const auto &mutant_operator : mutant_operators)
    {
        size_t operator_pos = clause.find(mutant_operator);
        if (operator_pos != string::npos)
        {
            for (const auto &replacement_operator : mutant_operators)
            {
                if (mutant_operator != replacement_operator)
                {
                    string mutated_query = query;
                    mutated_query.replace(start_pos + operator_pos, mutant_operator.length(), replacement_operator);

                    mutantTreeNode->AddMutantChildren(mutated_query);
                    mutationData.mutated_queries.push_back(mutated_query);
                }
            }
            break;
        }
    }
}

void ROR_OperatorComplexHelper(string query, MutationData &mutationData, TreeNode *mutantTreeNode, size_t start_pos, size_t end_pos)
{
    const string mutant_operators[] = {"<", "<=", "=", ">", ">=", "<>"};
    string lower_query = query;
    std::transform(lower_query.begin(), lower_query.end(), lower_query.begin(), ::tolower);

    size_t a = lower_query.rfind(" and ", end_pos - 1);
    size_t b = lower_query.rfind(" or ", end_pos - 1);

    if (a == string::npos)
    {
        if (b == string::npos)
        {
            string clause = query.substr(start_pos, end_pos - start_pos);

            for (const auto &mutant_operator : mutant_operators)
            {
                size_t operator_pos = clause.find(mutant_operator);
                if (operator_pos != string::npos)
                {
                    for (const auto &replacement_operator : mutant_operators)
                    {
                        if (mutant_operator != replacement_operator)
                        {
                            string mutated_query = query;
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

            string clause = query.substr(b + 4, end_pos_temp - (b + 4));

            for (const auto &mutant_operator : mutant_operators)
            {
                size_t operator_pos = clause.find(mutant_operator);
                if (operator_pos != string::npos)
                {
                    for (const auto &replacement_operator : mutant_operators)
                    {
                        if (mutant_operator != replacement_operator)
                        {
                            string mutated_query = query;
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
        if (b == string::npos)
        {
            ROR_OperatorComplexHelper(query, mutationData, mutantTreeNode, start_pos, a);

            size_t end_pos_temp = std::min(lower_query.find(" ", a + 5), lower_query.find(";", a + 5));

            string clause = query.substr(a + 5, end_pos_temp - (a + 5));

            for (const auto &mutant_operator : mutant_operators)
            {
                size_t operator_pos = clause.find(mutant_operator);
                if (operator_pos != string::npos)
                {
                    for (const auto &replacement_operator : mutant_operators)
                    {
                        if (mutant_operator != replacement_operator)
                        {
                            string mutated_query = query;
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

            string clause = query.substr(capped_pos + size, end_pos_temp - (capped_pos + size));

            for (const auto &mutant_operator : mutant_operators)
            {
                size_t operator_pos = clause.find(mutant_operator);
                if (operator_pos != string::npos)
                {
                    for (const auto &replacement_operator : mutant_operators)
                    {
                        if (mutant_operator != replacement_operator)
                        {
                            string mutated_query = query;
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

std::vector<std::string> splitOperators(const string &input)
{
    vector<string> result;
    std::istringstream stream(input);
    string word;
    while (stream >> word)
    {
        result.push_back(word);
    }
    return result;
}

void generatePermutationOfConjunction(int and_count, int or_count, string value, vector<vector<string>> &permutations)
{
    if (and_count == 0 || or_count == 0)
    {
        vector<string> result;
        std::istringstream stream(value);
        string word;
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

void Mutator::ROR_OperatorComplex(string query, MutationData &mutationData, TreeNode *mutantTreeNode, string clause)
{
    string lower_query = query;
    std::transform(lower_query.begin(), lower_query.end(), lower_query.begin(), ::tolower);
    size_t where_pos = lower_query.find(clause);
    size_t conjective_operator_count = countKeyword(lower_query, "and");
    conjective_operator_count += countKeyword(lower_query, "or");
    if (where_pos == string::npos || conjective_operator_count == 0)
        return;
    // cout << conjective_operator_count << endl;
    // cout << lower_query << endl;
    const string mutant_operators[] = {"<", "<=", "=", ">", ">=", "<>"};

    size_t search_start_pos = where_pos + clause.size();

    size_t last_and_pos = lower_query.rfind(" and ");
    last_and_pos = last_and_pos == string::npos ? -2 : last_and_pos;
    size_t last_or_pos = lower_query.rfind(" or ");
    last_or_pos = last_or_pos == string::npos ? -1 : last_or_pos;
    size_t last_conj_pos = std::max(last_and_pos + 5, last_or_pos + 4);
    size_t end_pos = lower_query.find(" ", last_conj_pos);
    end_pos = end_pos == string::npos ? lower_query.find(";", last_conj_pos) : end_pos;
    // size_t last_conj_pos = std::max(lower_query.rfind(" and ") + 5, lower_query.rfind(" or ") + 4);
    // size_t end_pos = lower_query.find(" ", std::max(lower_query.rfind(" and ") + 5, lower_query.rfind(" or ") + 4));

    vector<vector<string>> conjective_operator_permutations = {};
    generatePermutationOfConjunction(conjective_operator_count, conjective_operator_count, {}, conjective_operator_permutations);

    for (size_t j = 0; j < conjective_operator_permutations.size(); j++)
    {
        // conjective_operator_count;
        size_t count = 0;
        string mutated_query = query;
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

void Mutator::ROR_Operator(string query, MutationData &mutationData, TreeNode *mutantTreeNode)
{
    /**
     * @brief Mutate the relational operator (<,<=,=,>,>=,<>) with each other
     *
     */

    const string mutant_operators[] = {"<", "<=", "=", ">", ">=", "<>"};

    string lower_query = query;

    std::transform(lower_query.begin(), lower_query.end(), lower_query.begin(), ::tolower);

    size_t where_pos = lower_query.find(" where ");
    size_t having_pos = lower_query.find(" having ");

    if (where_pos != string::npos || having_pos != string::npos)
    {
        size_t and_pos = lower_query.find(" and ");
        size_t or_pos = lower_query.find(" or ");
        if (and_pos == string::npos && or_pos == string::npos)
        {
            size_t clause_start = (where_pos != string::npos) ? where_pos : having_pos;
            string clause = query.substr(clause_start);

            for (const auto &mutant_operator : mutant_operators)
            {
                size_t operator_pos = clause.find(mutant_operator);
                if (operator_pos != string::npos)
                {
                    for (const auto &replacement_operator : mutant_operators)
                    {
                        if (mutant_operator != replacement_operator)
                        {
                            string mutated_query = query;
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
            if (where_pos != string::npos)
                ROR_OperatorComplex(query, mutationData, mutantTreeNode, " where ");

            if (having_pos != string::npos)
                ROR_OperatorComplex(query, mutationData, mutantTreeNode, " having ");
        }
    }
    return;
}

size_t findClause(const string &query, const string &clause, size_t start_pos = 0, size_t end_pos = 0)
{
    if (end_pos == 0 || end_pos < start_pos)
    {
        end_pos = string::npos;
    }

    size_t character_counts_search = (end_pos == string::npos) ? (query.length() - start_pos) : end_pos - start_pos;
    size_t substring_pos = query.substr(start_pos, character_counts_search).find(clause);
    size_t pos = (substring_pos != string::npos) ? start_pos + substring_pos : string::npos;
    return (pos != string::npos && pos < end_pos) ? pos : query.length();
}

static string excludeClauses(const string &query, size_t group_by_pos, size_t order_by_pos)
{
    size_t exclude_start = std::min(group_by_pos, order_by_pos);
    return query.substr(0, exclude_start);
}

string excludeExistsSelects(const string &query, const std::regex &exists_regex)
{
    return std::regex_replace(query, exists_regex, ""); // Remove EXISTS subquery portions
}

string mutateStringWithNew(const string &query, size_t pos, const string &old_value, const string &new_value)
{
    string mutated_query = query;
    mutated_query.replace(pos, old_value.length(), new_value);
    return mutated_query;
}

tuple<string, int> findFirstChar(const string &query, int position, bool moveRight, size_t character_length = 1, size_t word_count = 1)
{
    if (position < 0 || position >= static_cast<int>(query.length()))
    {
        throw std::out_of_range("Position is out of range of the string.");
    }

    size_t mover = 0;
    if (moveRight)
    {
        mover = 1;
    }
    else
    {
        mover = -1;
    }

    string output = "";
    bool foundSecondSpace = false;
    bool foundFirstChar = false;
    size_t found_word_count = 0;
    for (size_t i = position + mover; i < query.length() - 1; i = i + mover)
    {
        if (foundFirstChar && foundSecondSpace && found_word_count >= word_count)
        {
            return std::make_tuple(output, i - (mover * 2));
        }

        if (!std::isspace(query[i]))
        {
            foundFirstChar = true;
            if (character_length > output.length() || character_length == 0)
            {
                if (moveRight)
                    output += query[i];
                else
                    output = query[i] + output;
            }
            else
            {
                return std::make_tuple(output, i - (mover * 2));
            }
        }
        else
        {
            if (foundFirstChar)
            {
                if (foundSecondSpace)
                {
                    output += " ";
                }
                found_word_count++;
                foundSecondSpace = true;
            }
        }
    }
    return std::make_tuple(output, query.length() - 1 - output.length());
}

void Mutator::UOI_Operator(string query, MutationData &mutationData, TreeNode *mutantTreeNode)
{
    /**
     * @brief Each arithmetic expression or reference to a number e is replaced by -e, e + 1 and e - 1. References to numbers are not mutated either inside of GROUP BY and ORDER BY clauses or in the select-list of an EXISTS subquery.
     *
     */

    string processed_query = query;
    string mutated_query = query;
    std::transform(processed_query.begin(), processed_query.end(), processed_query.begin(), ::tolower);
    std::transform(mutated_query.begin(), mutated_query.end(), mutated_query.begin(), ::tolower);

    std::regex number_regex(R"((\b\d+\b))");
    std::smatch match;

    size_t start_index = 0;

    std::sregex_iterator iter(processed_query.begin() + start_index, processed_query.end(), number_regex);
    std::sregex_iterator end;

    for (; iter != end; ++iter)
    {
        string number_str = iter->str();
        int number = std::stoi(number_str);
        size_t number_pos = iter->position() + start_index;
        if (std::get<0>(findFirstChar(processed_query, number_pos, false)) == "<" || std::get<0>(findFirstChar(processed_query, number_pos, false)) == ">" || std::get<0>(findFirstChar(processed_query, number_pos, false)) == "=")
        {
            string mutation_neg = mutateStringWithNew(mutated_query, number_pos, number_str, std::to_string(-number));
            mutantTreeNode->AddMutantChildren(mutation_neg);
            mutationData.mutated_queries.push_back(mutation_neg);
        }

        string mutation_add = mutateStringWithNew(mutated_query, number_pos, number_str, std::to_string(number + 2));
        mutantTreeNode->AddMutantChildren(mutation_add);
        mutationData.mutated_queries.push_back(mutation_add);

        string mutation_sub = mutateStringWithNew(mutated_query, number_pos, number_str, std::to_string(number - 2));
        mutantTreeNode->AddMutantChildren(mutation_sub);
        mutationData.mutated_queries.push_back(mutation_sub);
    }
}

void Mutator::AOR_Operator(string query, MutationData &mutationData, TreeNode *mutantTreeNode)
{
    /**
     * @brief Each arithmetic operator is replaced by each of the other arithmetic operators.
     *
     */

    string processed_query = query;
    std::transform(processed_query.begin(), processed_query.end(), processed_query.begin(), ::tolower);

    const string arithmetic_operators[] = {"+", "-", "*", "/"};
    for (size_t i = 0; i < processed_query.length(); i++)
    {
        switch (processed_query[i])
        {
        case '+':
        case '-':
        case '*':
        case '/':
        {
            for (auto replacing_operator : arithmetic_operators)
            {
                if (processed_query[i] == replacing_operator[0])
                    continue;

                if (std::isalnum(std::get<0>(findFirstChar(processed_query, i, false))[0]) && std::isalnum(std::get<0>(findFirstChar(processed_query, i, true))[0]))
                {
                    processed_query.replace(i, 1, replacing_operator);
                    // cout << "Operator: " << processed_query << endl;
                    mutantTreeNode->AddMutantChildren(processed_query);
                    mutationData.mutated_queries.push_back(processed_query);
                }
            }

            break;
        }

        default:
            break;
        }
    }
}

void Mutator::JOI_operator(string query, MutationData &mutationData, TreeNode *mutantTree)
{
    /**
     * LEFT JOIN
     * RIGHT JOIN
     * INNER JOIN
     * FULL JOIN
     * SEMI JOIN
     * ANTI JOIN
     */

    string lower_query = query;
    std::transform(lower_query.begin(), lower_query.end(), lower_query.begin(), ::tolower);
    size_t join_pos_exist = lower_query.find(" join ");
    if (join_pos_exist != string::npos)
    {
        // std::cout << "JOIN CLAUSE EXISTS: " << query << std::endl;
        // join clause exists
        mutantTree->AddMutantChildren(query);
        const string join_types[] = {"left", "right", "full", "semi", "anti"};
        size_t join_pos = lower_query.find("join");

        for (size_t i = 0; i < join_types->size(); i++)
        {
            const string &type = join_types[i];
            size_t type_pos = lower_query.rfind(type, join_pos - 1); // Search before "join"
            if (type_pos != string::npos)
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

void Mutator::BTW_Operator(string query, MutationData &mutationData, TreeNode *mutantTree)
{
    string lower_query = query;

    size_t between_position = findClause(lower_query, "between");
    size_t not_between_position = findClause(lower_query, "not between");

    if (between_position == lower_query.length())
    {
        return;
    }

    size_t and_position = findClause(lower_query, " and ", between_position + 6);
    string left_clause = std::get<0>(findFirstChar(lower_query, and_position + 1, false, 0, 1));
    auto [right_clause, right_clause_pos] = findFirstChar(lower_query, and_position + 3, true, 0, 1);

    if (not_between_position == lower_query.length())
    {
        auto [column_clause, column_clause_pos] = findFirstChar(lower_query, between_position, false, 0, 1);
        string mutated_clause = column_clause + ">" + left_clause + " AND " + column_clause + "<=" + right_clause;
        lower_query = query;
        lower_query.replace(column_clause_pos, right_clause_pos + right_clause.length() - column_clause_pos, mutated_clause);
        mutantTree->AddMutantChildren(lower_query);
        mutationData.mutated_queries.push_back(lower_query);

        mutated_clause = column_clause + ">=" + left_clause + " AND " + column_clause + "<" + right_clause;
        string lower_query = query;
        lower_query.replace(column_clause_pos, right_clause_pos + right_clause.length() - column_clause_pos, mutated_clause);
        mutantTree->AddMutantChildren(lower_query);
        mutationData.mutated_queries.push_back(lower_query);
    }
    else
    {
        auto [column_clause, column_clause_pos] = findFirstChar(lower_query, not_between_position, false, 0, 1);
        string mutated_clause = column_clause + "<" + left_clause + " OR " + column_clause + ">=" + right_clause;
        lower_query = query;
        lower_query.replace(column_clause_pos, right_clause_pos + right_clause.length() - column_clause_pos, mutated_clause);
        mutantTree->AddMutantChildren(lower_query);
        mutationData.mutated_queries.push_back(lower_query);

        mutated_clause = column_clause + "<=" + left_clause + " OR " + column_clause + ">" + right_clause;
        string lower_query = query;
        lower_query.replace(column_clause_pos, right_clause_pos + right_clause.length() - column_clause_pos, mutated_clause);
        mutantTree->AddMutantChildren(lower_query);
        mutationData.mutated_queries.push_back(lower_query);
    }
}

void Mutator::InternalMutate(string &query, MutationData &mutationData, TreeNode *mutantTreeNode, MutationOperator operatr)
{
    switch (operatr)
    {
    case MutationOperator::SELECT:
    {
        // std::cout << "Before select mutation for query: " << query << std::endl;
        SEL_operator(query, mutationData, mutantTreeNode);
        // std::cout << "After select mutation for query: " << query << std::endl;
        // std::cout << "Select Mutant Count: " << mutantTreeNode->children.size() << std::endl;
        if (mutantTreeNode->children.size() == 0)
        {
            InternalMutate(query, mutationData, mutantTreeNode, MutationOperator::JOIN);
        }
        else
        {

            for (auto childMutantTreeNode : mutantTreeNode->children)
            {
                // std::cout << "Mutant Select: " << childMutantTreeNode->query << std::endl;
                InternalMutate(childMutantTreeNode->query, mutationData, childMutantTreeNode, MutationOperator::JOIN);
            }
        }
        break;
    }
    case MutationOperator::JOIN:
    {
        JOI_operator(query, mutationData, mutantTreeNode);
        // std::cout << "After Join Mutation for: " << query << std::endl;
        // std::cout << "Join Mutant Count: " << mutantTreeNode->children.size() << std::endl;
        if (mutantTreeNode->children.size() == 0)
        {
            // InternalMutate(query, mutationData, mutantTreeNode, MutationOperator::RELATIONAL_OR);
            InternalMutate(query, mutationData, mutantTreeNode, MutationOperator::BTW_OP);
        }
        else
        {

            for (auto childMutantTreeNode : mutantTreeNode->children)
            {
                // std::cout << "Mutant Join: " << childMutantTreeNode->query << std::endl;
                InternalMutate(childMutantTreeNode->query, mutationData, childMutantTreeNode, MutationOperator::BTW_OP);
            }
        }
        // std::cout << "Internal Mutation Completed for JOIN" << std::endl;
        break;
    }
    case MutationOperator::BTW_OP:
    {
        BTW_Operator(query, mutationData, mutantTreeNode);
        if (mutantTreeNode->children.size() == 0)
            InternalMutate(query, mutationData, mutantTreeNode, MutationOperator::RELATIONAL_OR);
        else
        {
            for (auto childMutantTreeNode : mutantTreeNode->children)
            {
                InternalMutate(childMutantTreeNode->query, mutationData, childMutantTreeNode, MutationOperator::RELATIONAL_OR);
            }
        }
        break;
    }
    case MutationOperator::RELATIONAL_OR:
    {
        ROR_Operator(query, mutationData, mutantTreeNode);
        // std::cout << "ROR Mutant Count: " << mutantTreeNode->children.size() << std::endl;
        if (mutantTreeNode->children.size() == 0)
        {
            InternalMutate(query, mutationData, mutantTreeNode, MutationOperator::UNARY_IOR);
        }
        else
        {
            for (auto childMutantTreeNode : mutantTreeNode->children)
            {
                // std::cout << "Mutant ROR: " << childMutantTreeNode->query << std::endl;
                InternalMutate(childMutantTreeNode->query, mutationData, childMutantTreeNode, MutationOperator::UNARY_IOR);
            }
        }
        break;
    }
    case MutationOperator::UNARY_IOR:
    {
        UOI_Operator(query, mutationData, mutantTreeNode);

        if (mutantTreeNode->children.size() == 0)
        {
            InternalMutate(query, mutationData, mutantTreeNode, MutationOperator::LOGICAL_AOR);
        }
        else
        {
            for (auto childMutantTreeNode : mutantTreeNode->children)
            {
                InternalMutate(childMutantTreeNode->query, mutationData, childMutantTreeNode, MutationOperator::LOGICAL_AOR);
            }
        }
        break;
    }
    case MutationOperator::LOGICAL_AOR:
    {
        AOR_Operator(query, mutationData, mutantTreeNode);

        if (mutantTreeNode->children.size() == 0)
            InternalMutate(query, mutationData, mutantTreeNode, MutationOperator::LKE_OP);
        else
        {
            for (auto childMutantTreeNode : mutantTreeNode->children)
            {
                InternalMutate(childMutantTreeNode->query, mutationData, childMutantTreeNode, MutationOperator::LKE_OP);
            }
        }
        break;
    }
    case MutationOperator::LKE_OP:
    {
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
