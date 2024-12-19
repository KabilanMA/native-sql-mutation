#include "cli/CLI.h"
#include "core/Mutator.h"
#include "alias.h"
#include <thread>
#include <atomic>
#include "duckdb.h"
#include "core/Comparator.h"

int main()
{
    string query;
    std::atomic<bool> is_processing{false};

    while (true)
    {
        // cout << "mutant> ";
        // getline(cin, query);

        // if (query == "exit")
        // {
        //     std::cout << "Goodbye!" << endl;
        //     break;
        // }

        // is_processing = true;
        // // thread spinner_thread(CLI::Spinner, ref(is_processing));

        // MutationData mutationData(query);
        // CLI::ProcessQuery(mutationData);

        // // for (auto mutant : mutationData.mutated_queries)
        // // {
        // //     cout << mutant << endl;
        // // }

        // cout << "Total Mutant Count: " << mutationData.mutated_queries.size() << endl;

        duckdb_database db;
        duckdb_connection con;
        if (duckdb_open(NULL, &db) == DuckDBError)
        {
            cout << "DUCKDB database open error" << endl;
        }
        if (duckdb_connect(db, &con) == DuckDBError)
        {
            cout << "DUCKDB connection error" << endl;
        }

        // Additional queries to compare
        std::vector<std::string> additional_queries = {
            "CREATE TABLE my_table (id INT PRIMARY KEY, name VARCHAR(50) NOT NULL, age INT, address VARCHAR(100));",
            "INSERT INTO my_table (id, name, age, address) VALUES (1, 'Alice', 25, '123 Maple Street'), (2, 'Bob', 30, '456 Oak Avenue'), (3, 'Charlie', 22, '789 Pine Road'), (4, 'Diana', 27, '321 Elm Street'), (5, 'Ethan', 32, '654 Birch Lane'), (6, 'Fiona', 28, '987 Cedar Drive'), (7, 'George', 24, '741 Walnut Way'), (8, 'Hannah', 26, '852 Cherry Court'), (9, 'Ian', 29, '963 Ash Boulevard'), (10, 'Jack', 31, '369 Magnolia Circle'), (11, 'Kathy', 23, '147 Willow Lane'), (12, 'Liam', 30, '258 Maple Avenue'), (13, 'Mia', 21, '369 Spruce Street'), (14, 'Noah', 27, '123 Poplar Road'), (15, 'Olivia', 33, '456 Alder Court'), (16, 'Paul', 26, '789 Sycamore Lane'), (17, 'Quincy', 28, '321 Palm Drive'), (18, 'Rachel', 24, '654 Pine Avenue'), (19, 'Steve', 29, '987 Aspen Road'), (20, 'Tina', 25, '741 Beech Street');"};

        for (auto &query : additional_queries)
        {
            duckdb_result compare_result;
            if (duckdb_query(con, query.c_str(), &compare_result) != DuckDBSuccess)
            {
                std::cerr << "Failed to execute query: " << query << std::endl;
                continue;
            }

            duckdb_destroy_result(&compare_result);
        }

        query = "SELECT * FROM my_table WHERE id < 20;";
        duckdb_result original_result;
        if (duckdb_query(con, query.c_str(), &original_result) != DuckDBSuccess)
        {
            std::cerr << "Failed to execute the original query!" << std::endl;
            duckdb_disconnect(&con);
            duckdb_close(&db);
            return 1;
        }

        Comparator comparator(original_result);

        duckdb_result cmp_result;
        query = "SELECT * FROM my_table WHERE id < 10;";
        if (duckdb_query(con, query.c_str(), &cmp_result) != DuckDBSuccess)
        {
            std::cerr << "Failed to execute the original query!" << std::endl;
            duckdb_disconnect(&con);
            duckdb_close(&db);
            return 1;
        }

        cout << comparator.is_equal_to_original(cmp_result) << endl;

        // Clean up
        duckdb_destroy_result(&original_result);
        duckdb_destroy_result(&cmp_result);
        duckdb_disconnect(&con);
        duckdb_close(&db);

        // is_processing = false;
        // // spinner_thread.join();

        cout << "Done processing the Query: " << query << endl;
        break;
    }
    return 0;
}