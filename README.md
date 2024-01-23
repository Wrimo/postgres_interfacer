# What is this? 
C++ code that generates C++ code that calls stores procedures in a Postgresql database and converts the result into C++ struct. 

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
./gen init
```

This will compile the program and then run the program  and generate the new files. Notice the init parameter. When running with the init, the program will add stored procedures to the database to retrieve tables and functions. After the first execution of the program for a database, regenerate the code by running `./gen`. 

Running `make clean` will remove the executable and delete the generated files. 

# Limitations
- Only `text`, `integer`, `numeric`, and `boolean` types are supported. Defining what C++ type a Postgresql type should be mapped can be added to `convertDataType` in `code_gen.cpp`. 
