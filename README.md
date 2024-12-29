### Conditions

- All where clause conditions should not contain any space in between the comparison operator.
  - E.g.:
    - CORRECT -> SELECT col1, col2 FROM t1 LEFT JOIN t2 WHERE col1>5 AND col3<5;
    - INCORRECT-> SELECT col1, col2 FROM t1 LEFT JOIN t2 WHERE col1 > 5 AND col3<5;
- All arithme

### Sample SQL Statements

1. SELECT col1, col2 from t1 LEFT JOIN t2 ON t1.v=t2.r WHERE t1.col3>10 AND t1.col3<20;
2. SELECT col1, col2 from t1 LEFT JOIN t2 ON t1.v=t2.r WHERE t1.col3>10 AND t1.col3<20 OR t2.col>30;
3. SELECT col1, col2 from t1 LEFT JOIN t2 ON t1.v=t2.r HAVING t1.col3>10;
4. SELECT col1, col2 from t1 LEFT JOIN t2 ON t1.v=t2.r HAVING t1.col3>10 AND t1.col3<20 OR t2.col>30;
5. SELECT COUNT(CustomerID), Country FROM Customers GROUP BY Country;
6. SELECT department, COUNT(_) AS employee_count FROM employees WHERE salary>50000 GROUP BY department HAVING COUNT(_)>5;

7. CREATE TABLE my_table (id INT PRIMARY KEY, name VARCHAR(50) NOT NULL, age INT, address VARCHAR(100));
8. INSERT INTO my_table (id, name, age, address) VALUES (1, 'Alice', 25, '123 Maple Street'), (2, 'Bob', 30, '456 Oak Avenue'), (3, 'Charlie', 22, '789 Pine Road'), (4, 'Diana', 27, '321 Elm Street'), (5, 'Ethan', 32, '654 Birch Lane'), (6, 'Fiona', 28, '987 Cedar Drive'), (7, 'George', 24, '741 Walnut Way'), (8, 'Hannah', 26, '852 Cherry Court'), (9, 'Ian', 29, '963 Ash Boulevard'), (10, 'Jack', 31, '369 Magnolia Circle'), (11, 'Kathy', 23, '147 Willow Lane'), (12, 'Liam', 30, '258 Maple Avenue'), (13, 'Mia', 21, '369 Spruce Street'), (14, 'Noah', 27, '123 Poplar Road'), (15, 'Olivia', 33, '456 Alder Court'), (16, 'Paul', 26, '789 Sycamore Lane'), (17, 'Quincy', 28, '321 Palm Drive'), (18, 'Rachel', 24, '654 Pine Avenue'), (19, 'Steve', 29, '987 Aspen Road'), (20, 'Tina', 25, '741 Beech Street');
9. SELECT col1 FROM my_table WHERE id<20;
10. SELECT col1 FROM my_table WHERE id<10 ORDER BY name;
11. SELECT id, name FROM my_table WHERE id<10;
12. SELECT col1 FROM table WHERE id BETWEEN 10 AND 20;
13. SELECT id FROM table WHERE name LIKE 'kabilan';
14. SELECT address, AVG(DISTINCT age) AS avg_distinct_age FROM my_table GROUP BY address HAVING AVG(DISTINCT age)<28;
