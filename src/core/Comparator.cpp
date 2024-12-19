#include "core/Comparator.h"

Comparator::Comparator(duckdb_result &original)
{
    this->original_result = &original;
}

bool Comparator::is_equal_to_original(duckdb_result &compare_result)
{
    if ((duckdb_row_count(original_result) != duckdb_row_count(&compare_result)) || (duckdb_column_count(original_result) != duckdb_column_count(&compare_result)))
    {
        return false;
    }

    idx_t row_count = duckdb_row_count(original_result);
    idx_t column_count = duckdb_column_count(original_result);

    for (idx_t row = 0; row < row_count; row++)
    {
        for (idx_t col = 0; col < column_count; col++)
        {
            auto str_original_val = duckdb_value_varchar(original_result, col, row);
            auto str_cmp_val = duckdb_value_varchar(&compare_result, col, row);

            if (strcmp(str_original_val, str_cmp_val) != 0)
            {
                duckdb_free(str_original_val);
                duckdb_free(str_cmp_val);
                cout << "RESULT NOT MATCHING" << endl;
                return false;
            }
            duckdb_free(str_original_val);
            duckdb_free(str_cmp_val);
        }
    }
    return true;
}
