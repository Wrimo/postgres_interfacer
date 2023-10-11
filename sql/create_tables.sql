CREATE TABLE books 
(
    id integer, 
    title text, 
    author_id integer 
);
CREATE TABLE sales 
(
    id integer, 
    sales numeric,
    book_id integer
);
CREATE TABLE author
(
    id integer, 
    firstName text, 
    lastName text, 
    age int
);

