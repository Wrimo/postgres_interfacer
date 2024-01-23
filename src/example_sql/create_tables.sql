CREATE TABLE author
(
    id serial primary key, 
    firstName text, 
    lastName text, 
    age int
);
CREATE TABLE book
(
    id serial primary key, 
    title text, 
    author_id integer references author(id) 
);
CREATE TABLE sale 
(
    id serial primary key, 
    sales numeric,
    book_id integer references book(id)
);

INSERT INTO author(firstName, lastName, age) VALUES 
('Cormac', 'McCarthy', 90),
('Virgina', 'Woolf', 141); 

INSERT INTO book(title, author_id) VALUES 
('Blood Meridian', 1), 
('Mrs. Dalloway', 2);