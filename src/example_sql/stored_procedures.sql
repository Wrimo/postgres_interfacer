CREATE OR REPLACE FUNCTION get_authors()
RETURNS SETOF author
AS $$
select * 
from 
    author
$$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION get_authors_over_age(age integer) 
RETURNS SETOF author
AS $$
select * 
from 
    author
where age > $1
$$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION get_author_name_like(name text) 
RETURNS SETOF author
AS $$
select * 
from 
    author
where firstName like $1 || '%'
$$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION get_author_books()
RETURNS TABLE(name text, title text)
AS $$
select
    firstName || ' ' || lastName as name, 
    title
from
    author
inner join book on
    book.author_id = author.id
group by name, title;
$$ LANGUAGE SQL;

CREATE OR REPLACE PROCEDURE insert_author(firstName text, lastName text, age integer)
LANGUAGE SQL AS $$
INSERT INTO author(firstName, lastName, age) VALUES(firstName, lastName, age);
$$