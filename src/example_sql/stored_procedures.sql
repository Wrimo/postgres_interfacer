CREATE OR REPLACE FUNCTION get_authors()
RETURNS SETOF author
AS $$
select * 
from 
    author
$$ LANGUAGE SQL;RETURNS TABLE (id integer, firstName text, lastName text, age int)

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