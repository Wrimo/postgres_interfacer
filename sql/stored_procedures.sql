-- Gets all of the tables and their fields
-- Used to generate types in C++
CREATE FUNCTION get_tables_and_fields()
RETURNS TABLE(name text, fields text[])
as $$
select
    t.table_name::text,
    array_agg(c.column_name::text) as columns
from
    information_schema.tables t
inner join information_schema.columns c on
    t.table_name = c.table_name
where
    t.table_schema = 'public'
    and t.table_type= 'BASE TABLE'
    and c.table_schema = 'public'
group by t.table_name;
$$ LANGUAGE SQL