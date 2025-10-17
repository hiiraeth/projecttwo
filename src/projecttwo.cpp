#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <climits>
#include <limits>
#include <algorithm>

struct Course
{
    std::string courseID;
    std::string courseTitle;
    std::vector<std::string> prerequisites;

    Course()
    {
        courseID = "0";
        courseTitle = "Title";
    }

    Course(std::string id, std::string title, std::vector<std::string> prereqs)
    {
        courseID = id;
        courseTitle = title;
        prerequisites = prereqs;
    }
};

class HashTable 
{

private:
    struct Node
    {
        Course course;
        unsigned int key;
        Node* next;

        // default
        Node()
        {
            key = UINT_MAX;
            next = nullptr;
        }

        // init with a course
        Node(Course aCourse) : Node()
        {
            course = aCourse;
        }

        // init with a course and a key
        Node(Course aCourse, unsigned int aKey) : Node(aCourse)
        {
            key = aKey;
        }
    };

    unsigned int tableSize = 27;

    int hash(int key);

    std::vector<Node> nodes;


public:
    HashTable();
    HashTable(unsigned int sizeOfTable);
    ~HashTable();

    void InsertCourse(Course course);
    void PrintAllCourses();
    void Search(std::string id);
};

HashTable::HashTable()
{
    nodes.resize(tableSize);
}

HashTable::HashTable(unsigned int sizeOfTable)
{
    tableSize = sizeOfTable;
    nodes.resize(tableSize);
}

HashTable::~HashTable()
{
    for (auto& head : nodes)
    {
        Node* current = head.next;

        while (current != nullptr)
        {
            Node* next = current->next;
            delete current;
            current = next;
        }

        head.next = nullptr;
        head.key = UINT_MAX;
    }
}

int HashTable::hash(int key)
{
    return key % tableSize;
}

void HashTable::InsertCourse(Course course)
{
    std::string courseIDnumber = course.courseID.substr(4, 3);
    int courseIDnum = atoi(courseIDnumber.c_str());

    int bucketKey = hash(courseIDnum);

    Node& oldNode = nodes.at(bucketKey);

    if (oldNode.key == UINT_MAX)
    {
        oldNode.key = bucketKey;
        oldNode.course = course;
        oldNode.next = nullptr;
        return;
    }
    else
    {
        Node* currentNode = &oldNode;

        while (currentNode->next)
        {
            currentNode = currentNode->next;
        }

        currentNode->next = new Node(course, bucketKey);
        currentNode->next->next = nullptr;
    }
}

void HashTable::PrintAllCourses()
{
    std::vector<Course> allCourses;

    for (auto& elem : nodes)
    {
        allCourses.push_back(elem.course);
    }

    std::sort(allCourses.begin(), allCourses.end(), [](auto a, auto b)
    {
        return a.courseID < b.courseID;
    });

    for (auto& elem : allCourses)
    {
        if (elem.courseID == "0" || elem.courseTitle == "Title")
            continue;
            
        std::cout << elem.courseID << ", " <<
            elem.courseTitle << "\n";
    }
    
    // for (int i = 0; i < nodes.size(); i++)
    // {
    //     Node& head = nodes.at(i);
    //     if (head.key == UINT_MAX)
    //         continue;

    //     Node* current = &head;
    //     while (current != nullptr)
    //     {
    //         std::cout << current->course.courseID << ", " <<
    //             current->course.courseTitle << "\n";
            
    //         current = current->next;
    //     }
    // }

    std::cout << "\n";
}

void HashTable::Search(std::string id)
{
    int key = hash(atoi(id.substr(4,3).c_str()));

    Node* searchNode = &nodes.at(key);
    std::vector<std::string> prereqs = searchNode->course.prerequisites;

    std::cout << searchNode->course.courseID << ", " << searchNode->course.courseTitle << "\n";
    std::cout << "Prerequisites: ";

    if (!(prereqs.empty()))
    {
        for (std::string& elem : prereqs)
        {
            if (elem == *(prereqs.end() - 1))
                std::cout << elem;
            else
                std::cout << elem << ", ";
        }
    }
    else
    {
        std::cout << "None";
    }

    std::cout << "\n\n";
}

void trim(std::string& item)
{
    int start = 0;
    while (start < item.size() && (item[start] == ' ' || item[start] == '\t'))
    {
        start++;
    }

    int end = item.size();
    while (end > start && (item[end - 1] == ' ' || item[end - 1] == '\r' || 
            item[end - 1] == '\t'))
    {
        --end;
    }

    if (start > 0 || end < item.size())
        item = item.substr(start, end - start);
}

/**
 * Load the CSV file into the hashtable
 * 
 * @param csvPath           path of CSV file to read from
 * @param courses           hashtable of courses
 * @param courseids         vector of all course IDs
 * 
 * @return courses          returns hashtable by reference
 * @return validatePrereqs  returns prereqs that need validation
 */
void loadFileIntoHashtable(std::string csvPath, HashTable* courses, std::vector<std::string>& courseids) 
{
    std::ifstream inFile(csvPath);
    std::vector<std::string> row, prereqs;
    std::string line, item;

    if (!inFile.is_open())
    {
        std::cerr << "ERROR: File failed to open!" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    std::string headerLine;
    std::getline(inFile, headerLine);

    while (inFile.good())
    {
        std::getline(inFile, line, ',');
        
        if (line == "" || line == "\r")
            continue;
        else
            courseids.push_back(line);

        inFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    inFile.clear();
    inFile.seekg(0);

    std::getline(inFile, headerLine);

    int rowCount = 1;
    while (inFile.good())
    {
        row.clear();
        prereqs.clear();

        std::getline(inFile, line);
        std::stringstream ss_cut(line);

        while (std::getline(ss_cut, item, ','))
        {
            trim(item);

            if (item.empty())
                continue;

            row.push_back(item);
        }

        rowCount++;
        if (row.size() < 2)
        {
            std::cerr << "ERROR: Row needs at least 2 values. Row " << rowCount << " not read!" << std::endl;
            continue;
        }

        std::string id = row[0];
        std::string name = row[1];

        if (row.size() > 2)
        {
            for (int i = 2; i < row.size(); i++)
            {
                for (int j = 0; j < courseids.size(); j++)
                {
                    if (row.at(i) == courseids.at(j))
                    {
                        prereqs.push_back(row.at(i));
                    }
                }
            }
        }

        Course newCourse = Course(id, name, prereqs);

        courses->InsertCourse(newCourse);
    }

    inFile.close();
}

void showMenu()
{
    std::cout << "|--------------------------\n";
    std::cout << "|- 1. Load Data Structure"        << std::endl;
    std::cout << "|- 2. Print Course List"          << std::endl;
    std::cout << "|- 3. Print Course"               << std::endl;
    std::cout << "|- 9. Exit program"               << std::endl;
    std::cout << "|--------------------------\n";
    std::cout << "|- Enter choice: ";
}

bool validateID(std::vector<std::string>& courseids, std::string choice)
{
    trim(choice);

    auto it = std::find(courseids.begin(), courseids.end(), choice);

    if (it == courseids.end())
    {
        std::cerr << "ERROR: Invalid course.\n\n";
        return false;
    }

    return true;
}

int menuChoice(int choice, HashTable& courses, bool& load, std::vector<std::string>& courseids)
{
    const int MIN = 1;
    const int MAX = 3;
    const int EXIT = 9;

    std::string idChoice = "";
    
    std::cin >> choice;
    std::cout << "\n";
    
    if (choice < MIN || (choice > MAX && choice < EXIT) || choice > EXIT || std::cin.fail())
    {
        std::cout << "Invalid choice!\n\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    switch (choice)
    {
        case 1:
            if (load)
            {
                std::cerr << "ERROR: File already loaded.\n\n";
                break;
            }

            loadFileIntoHashtable("ABCU_Courses.csv", &courses, courseids);
            std::cout << "File loaded successfully.\n\n";
            load = true;

            break;

        case 2:
            if (!load)
            {
                std::cerr << "ERROR: Data structure not loaded.\n\n";
                break;
            }

            courses.PrintAllCourses();
            break;

        case 3: {
            if (!load)
            {
                std::cerr << "ERROR: Data structure not loaded.\n\n";
                break;
            }
            
            std::cout << "|- Enter course ID (ex: CSCI100): ";
            std::cin >> idChoice;
            std::cout << "\n";

            if (!validateID(courseids, idChoice))
                break;
            
            courses.Search(idChoice);
            break;
        }

        case 9:
            std::cout << "Thanks for using the program." << std::endl;
            break;

        default:
            break;
    }

    return choice;
}

int main()
{
    int choice = 0;
    std::vector<std::string> courseids;
    HashTable courses;
    bool isLoaded = false;

    while (choice != 9)
    {
        showMenu();
        choice = menuChoice(choice, courses, isLoaded, courseids);
    }

    return 0;
}