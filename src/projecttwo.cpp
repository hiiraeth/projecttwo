// --- Includes --- ///

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <climits>
#include <limits>
#include <algorithm>

// --- Object set up --- ///

/**
 * @brief Constructs a course object
 */
struct Course
{
    std::string courseID;
    std::string courseTitle;
    std::vector<std::string> prerequisites;

    // ctors
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

/**
 * @brief Constructs a hashtable object
 */
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
    // ctors and dtor
    HashTable();
    HashTable(unsigned int sizeOfTable);
    ~HashTable();

    // methods
    void InsertCourse(Course course);
    void PrintAllCourses();
    void Search(std::string id);
};

// --- Method definitions --- ///

/**
 * @brief HashTable default ctor
 */
HashTable::HashTable()
{
    nodes.resize(tableSize);
}

/**
 * @brief Hashtable ctor with table size
 */ 
HashTable::HashTable(unsigned int sizeOfTable)
{
    tableSize = sizeOfTable;
    nodes.resize(tableSize);
}

/**
 * @brief Hashtable dtor
 */
HashTable::~HashTable()
{
    // deletes all elements from hashtable
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

/**
 * @brief Hashing function 
 * 
 * Function will take in a key var (course's courseID),
 * and return a key for the hashtable by moding the key
 * by hashtable size
 * 
 * @param key   variable to hash
 * 
 * @return      returns hashed key
 */
int HashTable::hash(int key)
{
    return key % tableSize;
}

/**
 * @brief Insert course to hashtable
 * 
 * Function will first hash the courseID and implement
 * the course into the table using said key. Chains course
 * into bucket if the key is already in use by another
 * course.
 * 
 * @param course    course to insert
 */
void HashTable::InsertCourse(Course course)
{
    std::string courseIDnumber = course.courseID.substr(4, 3);
    int courseIDnum = atoi(courseIDnumber.c_str());

    int bucketKey = hash(courseIDnum);

    Node& oldNode = nodes.at(bucketKey);

    // checks if node is in use. if not, assign data to node
    if (oldNode.key == UINT_MAX)
    {
        oldNode.key = bucketKey;
        oldNode.course = course;
        oldNode.next = nullptr;
        return;
    }
    else // otherwise find next empty node
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

/**
 * @brief Print all courses in hashtable, sorted
 * 
 * Sorts all elements in hashtable based on their
 * courseID, and will then output them in order
 */
void HashTable::PrintAllCourses()
{
    std::vector<Course> allCourses;

    // inputs all courses into a vector
    for (auto& elem : nodes)
    {
        allCourses.push_back(elem.course);
    }

    // lambda function to sort all elements
    // variation of the following: https://stackoverflow.com/a/60753664
    std::sort(allCourses.begin(), allCourses.end(), [](auto a, auto b)
    {
        return a.courseID < b.courseID;
    });

    // outputs each sorted elements, ignoring default cases
    for (auto& elem : allCourses)
    {
        if (elem.courseID == "0" || elem.courseTitle == "Title" || elem.courseID == "courseID")
            continue;

        std::cout << elem.courseID << ", " <<
            elem.courseTitle << "\n";
    }

    std::cout << "\n";
}

/**
 * @brief Search for course
 * 
 * Searches for course in hashtable using the
 * courseID. Hashes the ID to find its key, and
 * then looks up the course at the key's
 * corresponding node and outputs its data.
 * 
 * @param id    id used to search
 */
void HashTable::Search(std::string id)
{
    int key = hash(atoi(id.substr(4,3).c_str()));

    Node* searchNode = &nodes.at(key);
    std::vector<std::string> prereqs = searchNode->course.prerequisites;

    std::cout << searchNode->course.courseID << ", " << searchNode->course.courseTitle << "\n";
    std::cout << "Prerequisites: ";

    // checking for prereqs
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

// --- Function definitions --- //

/**
 * @brief Trims courseID
 * 
 * Removes trailing and leading whitespace in the
 * courseID, specifically spaces and tabs from the
 * left and spaces, tabs, and '\r' from the right.
 * Will then trim the courseID and return it by ref.
 * 
 * @param item  item (courseID) to trim
 * 
 * @return returns trimmed courseID by reference
 */
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
 * @brief Load CSV file into hashtable
 * 
 * Traverses through the CSV file and adds data
 * from each line into temporary row. It first reads
 * through the file and saves each courseID for later
 * validation. It then reads through a second time and
 * cuts the data into parts using commas as delimiters.
 * It saves the information in id and name, and validates
 * the prerequisites by checking to make sure the course
 * exists already. 
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

    // reads in all courseids, saves for later validation
    while (inFile.good())
    {
        std::getline(inFile, line, ',');
        
        if (line == "" || line == "\r")
            continue;
        else
            courseids.push_back(line);

        // only reads the first column, aka where the course id is
        inFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    // resets inFile for second read
    inFile.clear();
    inFile.seekg(0);

    int rowCount = 1; // for error checking
    while (inFile.good())
    {
        // reads through each line, resets temp vars each iter
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

        // prerequisite validation
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

        // add course to hashtable
        Course newCourse = Course(id, name, prereqs);

        courses->InsertCourse(newCourse);
    }

    inFile.close();
}

/**
 * @brief Shows menu to user
 */
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

/**
 * @brief Validates courseID for searching
 * 
 * Function first trims users input, and then
 * checks the input against a vector of all
 * available courseIDs. Returns true if user's
 * search is in the vector, returns false if
 * search is NOT in the vector. Will then search
 * and output the course info using Search().
 * 
 * @param courseids all course ids
 * @param choice    user's search input
 * 
 * @return true if valid course, false if invalid course
 */
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

/**
 * @brief Main logic loop
 * 
 * Validates user input for the choice of menu
 * item. Then proceeds with the respective functions
 * based on user input, with input validation at
 * each case.
 * 
 * @param choice        user input, menu choice
 * @param csvPath       path to CSV file to open
 * @param courses       hashtable of all courses
 * @param load          check if courses have already been loaded
 * @param courseids     vector of all courseids for searching
 * 
 * @return returns user choice for main while loop
 */
int menuChoice(int choice, std::string& csvPath, HashTable& courses, bool& load, std::vector<std::string>& courseids)
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

            loadFileIntoHashtable(csvPath, &courses, courseids);
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

/**
 * @brief Processes command line arguments
 * 
 * @param argc      number of args passed
 * @param argv      array of string args
 * @param csvPath   path to CSV file
 */
void processCLIargs(int argc, char* argv[], std::string& csvPath)
{
    switch (argc) {
        case 2:
            csvPath = argv[1];
            break;
        default:
            csvPath = "CS 300 ABCU_Advising_Program_Input.csv";
            break;
    }
}

// --- Main loop --- //

int main(int argc, char* argv[])
{
    int choice = 0;
    std::string csvPath;
    std::vector<std::string> courseids;
    HashTable courses;
    bool isLoaded = false;

    processCLIargs(argc, argv, csvPath);

    while (choice != 9)
    {
        showMenu();
        choice = menuChoice(choice, csvPath, courses, isLoaded, courseids);
    }

    return 0;
}