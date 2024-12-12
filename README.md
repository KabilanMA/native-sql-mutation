### Conditions

- All where clause conditions should not contain any space in between the comparison operator.
  - E.g.:
    - CORRECT -> SELECT col1, col2 FROM t1 LEFT JOIN t2 WHERE col1>5 AND col3<5;
    - INCORRECT-> SELECT col1, col2 FROM t1 LEFT JOIN t2 WHERE col1 > 5 AND col3<5;
