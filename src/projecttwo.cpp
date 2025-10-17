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

void readInFile(std::string csvPath) 
{
    std::ifstream inFile;
    std::vector<std::string> row;
    std::string line;
    std::string item;

    std::vector<std::string> validatePrereqs;

    inFile.open(csvPath);

    while (inFile.good())
    {
        row.clear();

        std::getline(inFile, line);
        std::stringstream ss_cut(line);

        while (std::getline(ss_cut, item, ','))
        {
            row.push_back(item);
        }

        if (row.size() < 2)
        {
            std::cerr << "ERROR: Row needs at least 2 values" << std::endl;
            continue;
        }

        std::string id = row[0];
        std::string name = row[1];

        if (row.size() > 2)
        {
            for (auto prereq : row)
            {
                validatePrereqs.push_back(prereq);
            }
        }
    }
}

int main()
{

    std::cout << "test";
    return 0;
}