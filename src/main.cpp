#include <iostream>
#include <sstream>
#include <fstream>
#include "hidden.h"
#include "code_gen.h"

int main(int argc, char *argv[])
{
    std::stringstream conString;
    conString << "postgresql://" << USERNAME << ":" << PASSWORD << "@localhost:" << PORT << "/" << USERNAME;
    pqxx::connection c{conString.str()};
    pqxx::work txn{c};

    LanguageImplementation * lang = nullptr; 

    if (argc == 2 && std::string(argv[1]) == "init")
    {
        std::ifstream procedureScript("private_procedures.sql");
        std::string command((std::istreambuf_iterator<char>(procedureScript)),
                           std::istreambuf_iterator<char>());
        txn.exec(command);
        txn.commit(); 
        return 0; 
    }
    else if (argc < 2) 
    {
        std::cout << "usage: ./pinter language [file_path]\n";
        std::cout << "languge options: {cpp, py, rs}\n";
        return 1; 
    }
    else if (argc > 1 && std::string(argv[1]) == "cpp")
    {
        lang = new CPPImplementation();
    }
    else if (argc > 1 && std::string(argv[1]) == "py")
    {
        lang = new PythonImplementation();
    }
    else if (argc > 1 && std::string(argv[1]) == "rs")
    {
        lang = new RustImplementation();
    }
    else if (argc > 1)
    {
        std::cout << "Bad argument " << std::string(argv[1]) << "\n";
        return 1;
    }

    std::string path = "";
    if (argc == 3) 
    {   
        path = argv[2];
    }

    codeGenStart(lang, txn, path);


    txn.commit();

    return 0;
}