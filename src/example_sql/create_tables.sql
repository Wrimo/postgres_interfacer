CREATE TABLE book
(
    id integer, 
    title text, 
    author_id integer 
);
CREATE TABLE sale 
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

INSERT INTO book VALUES 
(0, 'Blood Meridian', 0), 
(1, 'Mrs. Dalloway', 1);

INSERT INTO author VALUES 
(0, 'Cormac', 'McCarthy', 90),
(1, 'Virgina', 'Woolf', 141); 