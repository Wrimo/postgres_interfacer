#include <string>

struct author{
std::string lastname;
std::string firstname;
int id;
int age;
};

struct book{
int author_id;
int id;
std::string title;
};

struct sale{
int id;
int book_id;
double sales;
};
