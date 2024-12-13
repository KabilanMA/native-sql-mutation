#include "cli/CLI.h"
#include "core/Mutator.h"
#include "alias.h"
#include <thread>
#include <atomic>
#include "duckdb.h"

// void compareResults(duckdb_result &original, duckdb_result &to_compare)
// {
//     // Check column count
//     if (original.column_count != to_compare.column_count)
//     {
//         std::cout << "Column count mismatch!" << std::endl;
//         return;
//     }

//     // Check row count
//     if (original.row_count != to_compare.row_count)
//     {
//         std::cout << "Row count mismatch!" << std::endl;
//         return;
//     }

//     // Compare row-by-row and column-by-column
//     for (idx_t row = 0; row < original.row_count; row++)
//     {
//         for (idx_t col = 0; col < original.column_count; col++)
//         {
//             auto original_value = duckdb_value_varchar(&original, col, row);
//             auto compare_value = duckdb_value_varchar(&to_compare, col, row);

//             if (std::string(original_value) != std::string(compare_value))
//             {
//                 std::cout << "Mismatch at row " << row << ", column " << col << "!" << std::endl;
//                 std::cout << "Original: " << original_value << ", To Compare: " << compare_value << std::endl;
//             }

//             duckdb_free(original_value);
//             duckdb_free(compare_value);
//         }
//     }
//     std::cout << "Comparison complete, no mismatches found!" << std::endl;
// }

int main()
{
    string query;
    std::atomic<bool> is_processing{false};

    while (true)
    {
        cout << "mutant> ";
        getline(cin, query);

        if (query == "exit")
        {
            std::cout << "Goodbye!" << endl;
            break;
        }

        is_processing = true;
        // thread spinner_thread(CLI::Spinner, ref(is_processing));

        MutationData mutationData(query);
        CLI::ProcessQuery(mutationData);

        for (auto mutant : mutationData.mutated_queries)
        {
            cout << mutant << endl;
        }

        cout << "Total Mutant Count: " << mutationData.mutated_queries.size() << endl;

        // duckdb_database db;
        // duckdb_connection con;
        // if (duckdb_open(NULL, &db) == DuckDBError)
        // {
        //     cout << "DUCKDB database open error" << endl;
        // }
        // if (duckdb_connect(db, &con) == DuckDBError)
        // {
        //     cout << "DUCKDB connection error" << endl;
        // }

        // query = "SELECT * FROM my_table WHERE id < 10;";
        // duckdb_result original_result;
        // if (duckdb_query(con, query.c_str(), &original_result) != DuckDBSuccess)
        // {
        //     std::cerr << "Failed to execute the original query!" << std::endl;
        //     duckdb_disconnect(&con);
        //     duckdb_close(&db);
        //     return 1;
        // }

        // // Additional queries to compare
        // std::vector<std::string> additional_queries = {
        //     "SELECT * FROM my_table WHERE id < 20;",
        //     "SELECT * FROM my_table WHERE id < 10 ORDER BY name;",
        //     "SELECT id, name FROM my_table WHERE id < 10;"};

        // for (auto &query : additional_queries)
        // {
        //     duckdb_result compare_result;
        //     if (duckdb_query(con, query.c_str(), &compare_result) != DuckDBSuccess)
        //     {
        //         std::cerr << "Failed to execute query: " << query << std::endl;
        //         continue;
        //     }

        //     // Compare results
        //     std::cout << "Comparing with query: " << query << std::endl;
        //     // compareResults(original_result, compare_result);

        //     // Clean up result
        //     duckdb_destroy_result(&compare_result);
        // }

        // // Clean up
        // duckdb_destroy_result(&original_result);
        // duckdb_disconnect(&con);
        // duckdb_close(&db);

        // is_processing = false;
        // spinner_thread.join();

        // cout << "Done processing the Query: " << query << endl;
    }
    return 0;
}