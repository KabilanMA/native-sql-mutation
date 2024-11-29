#include "cli/CLI.h"
#include "core/Mutator.h"
#include "alias.h"
#include <thread>
#include <atomic>
// #include "duckdb.h"

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
        thread spinner_thread(CLI::Spinner, ref(is_processing));

        MutationData mutationData(query);
        CLI::ProcessQuery(mutationData);

        // duckdb_database db;
        // duckdb_connection con;
        // if (duckdb_open(NULL, &db) == DuckDBError)
        // {
        //     cout << "DUCKDB connection error" << endl;
        // }

        // duckdb_result original_result;
        // duckdb_query(con, "CREATE TABLE test (id INTEGER, name VARCHAR);INSERT INTO test VALUES (1, 'Alice'), (2, 'Bob');SELECT * FROM test;", &original_result);

        // duckdb_destroy_result(&original_result);

        for (auto mutants : mutationData.mutated_queries)
        {
            std::cout << mutants << std::endl;
        }

        is_processing = false;
        spinner_thread.join();

        // cout << "Done processing the Query: " << query << endl;
    }
    return 0;
}