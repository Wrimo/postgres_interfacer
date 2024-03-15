# What is this? 
C++ code that generates code (see supported languages) that calls stores procedures in a Postgresql database and converts the result into C++ struct. 

The goal of this project is to enable database script to be written in SQL and be called in C++ without using a literal string everytime or manually creating C++ functions to manage the queries. 

# How do I use it? 
This project is built on top of [libpqxx](https://pqxx.org/development/libpqxx/#the-c-connector-for-postgresql) to connect with Postgresql and allows a further level of abstraction. 

Once libpqxx is installed and a Postgresql database is setup with tables and stored procedures, added a filed named `hidden.h` to the `src` folder and specify the username, password, and port for the database: 

```
#define USERNAME "user" 
#define PASSWORD "password"
#define PORT "xxxx"
```

Then run the following commands in the src directory:

```
make
./pinter init
```

This loads stored procedures into the database nessecary for information about tables and stored procedures. After this, the program can be run with 

``` 
./pinter language [file_path]
```

Running `make clean` will remove the executable and delete the generated files. 

# Supported Target Languages 
Can generate C++, Rust, or Python code. Additional languages can be added by creating a new override of the LanguageImplementation class. 

# Limitations
- Only `text`, `integer`, `numeric`, and `boolean` types are supported. Type conversions are specified per language in `convertDataType`.
- Doesn't support functions that returns scalar values. Only `SETOF` and `RETURNS TABLE` are supported.

# Future improvement 
- When running ./gen init, prompt for database login info and then use that to setup database. Save info for future use. 
- Cleanup python generated code 