#include <string>

struct author
{
    std::string lastname;
    std::string firstname;
    int id;
    int age;
};

struct book
{
    int id;
    std::string title;
    int author_id;
};

struct sale
{
    int book_id;
    double sales;
    int id;
};
