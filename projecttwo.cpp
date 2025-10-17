#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <stdint.h>

struct Course
{
    std::string courseID;
    std::string courseTitle;
    std::vector<std::string> prerequisites;
    std::vector<std::string> validatePrereqs;
};

class HashTable 
{

private:
    struct Node
    {
        const unsigned int MAX = __UINT8_MAX__;

        Course course;
        unsigned int key;
        Node* next;

        // default
        Node()
        {
            key = MAX;
            next = nullptr;
        }

        // init with a course
        Node(Course aCourse)
        {
            course = aCourse;
        }

        // init with a course and a key
        Node(Course aCourse, unsigned int aKey)
        {
            course = aCourse;
            key = aKey;
        }
    };

    unsigned int tableSize = 50;

    unsigned int hash(int key);

    std::vector<Node> nodes;

public:
    HashTable();
    HashTable(unsigned int sizeOfTable);
    ~HashTable();

    void InsertCourse(Course course);
    void RemoveCourse(std::string id);
    void PrintAllCourses();
    void Search(std::string id);
};

void readInFile(std::ifstream inFile) 
{
    
}

int main()
{


    return 0;
}