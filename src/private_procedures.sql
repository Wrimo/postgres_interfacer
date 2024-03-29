-- this file is used by main.cpp to create stored procedures that retrieve functions and tables 
CREATE SCHEMA private; -- used to prevent code gen for some stored procedures
SET SCHEMA 'private';
-- Gets all of the tables and their fields
-- Used to generate types in C++
CREATE OR REPLACE FUNCTION get_tables_and_fields()
RETURNS TABLE(name text, fields text[])
AS $$
select
    t.table_name::text,
    array_agg(c.column_name::text || ' ' || c.data_type order by c.ordinal_position) as columns
from
    information_schema.tables t
inner join information_schema.columns c on
    t.table_name = c.table_name
where
    t.table_schema = 'public'
    and t.table_type= 'BASE TABLE'
    and c.table_schema = 'public'
group by t.table_name;
$$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION get_stored_procedures()
RETURNS TABLE (name text, output text, input text)
AS $$ 
select 
    p.proname::text as name, 
    pg_catalog.pg_get_function_result(p.oid)::text, 
    pg_catalog.pg_get_function_arguments(p.oid)::text
from 
    pg_proc p 
    left join pg_namespace n on p.pronamespace = n.oid 
where 
    n.nspname in ('public')
group by name, p.oid; 
$$ LANGUAGE SQL; 

SET SCHEMA 'public';