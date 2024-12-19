#include "duckdb.h"
#include <cstring>
#include "alias.h"

class Comparator
{
private:
    duckdb_result *original_result;

public:
    Comparator(duckdb_result &original);
    ~Comparator() {};

public:
    bool is_equal_to_original(duckdb_result &compare_result);
};
