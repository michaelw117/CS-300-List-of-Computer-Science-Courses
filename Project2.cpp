// Project2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <algorithm>
#include <climits>
#include <iostream>
#include <fstream>
#include <istream>
#include <sstream>
#include <string>
#include <vector>
#include <ctype.h>

using namespace std;

struct Course {
    string courseNumber;
    string courseName;
    bool hasPrerequisites;
    vector<string> prerequisites;

    string getInformation() {
        return courseNumber + ", " + courseName;
    }
    Course() {
        hasPrerequisites = false;
    }
};

class HashTable {

private:

    struct Node {
        Course course;
        unsigned int key;
        Node* next;

        Node() {
            key = UINT_MAX;
            next = nullptr;
        }

        Node(Course pcourse) {
            key = UINT_MAX;
            next = nullptr;
            course = pcourse;
        }

        Node(Course pcourse, unsigned int pkey) {
            key = UINT_MAX;
            next = nullptr;
            course = pcourse;
            key = pkey;
        }
    };

    vector<Node> nodes;

    unsigned int tableSize;
    vector<Course> sortedCourses;
    unsigned int hash(long key);
    void quickSort(vector<Course>& sortedCourses, int begin, int end);
    int partition(vector<Course>& courses, int begin, int end);

public:
    HashTable();
    virtual ~HashTable();
    void Insert(Course course);
    void PrintAllCourses();
    Course Search(string courseNumber);
};

//constructor
HashTable::HashTable() {
    this->tableSize = 1000;
    nodes.resize(tableSize);
}

//destructor for hashtable
HashTable::~HashTable() {
    nodes.erase(nodes.begin());
}

unsigned int HashTable::hash(long key) {
    return(key % tableSize);
}

void HashTable::quickSort(vector<Course>& sortedcourses, int begin, int end) {
    if (begin >= end) {
        return;
    }

    //Partition bids into low an high
    int mid = partition(sortedcourses, begin, end);

    // recursively sort low partition (begin to mid (above))
    quickSort(sortedcourses, begin, mid);
    // recursively sort high partition (mid+1 to end)
    quickSort(sortedcourses, mid + 1, end);
}

int HashTable::partition(vector<Course>& courses, int begin, int end) {
    //set low and high to begin and end
    int low = begin;
    int high = end;

    // choose the middle as pivot point
    Course pivot = courses.at((low + (high - low)/ 2));

    // while not done
    bool done = false;
	while (!done) {

		// keep incrementing low index while bids[low] < bids[pivot]
		while (courses.at(low).courseNumber.compare(pivot.courseNumber) < 0) {
			low += 1;
		}
		// keep decrementing high index while bids[pivot] < bids[high]
		while (pivot.courseNumber.compare(courses.at(high).courseNumber) < 0) {
			high -= 1;
		}

		//If there are zero or one elements remaining, all bids are partitioned, then returns high
		if (low >= high) {
			done = true;
		}
		else {
			// else swaps the low and high bids (built in vector method)
			swap(courses.at(low), courses.at(high));
			// move low and high closer ++low, --high
			low += 1;
			high -= 1;
		}

    }
    //return high;
    return high;
}

void HashTable::Insert(Course course) {
    //it was hard for me to be able to hash words with digits, so I turned the first couple letters into ascii
    string toString ="";
    int intString = 0;
    for (int i = 0; i < course.courseNumber.size(); i++) {
        while (!isdigit(course.courseNumber[i])) {
            intString = intString + (int(course.courseNumber[i]));
            i++;
        }
        toString = toString + course.courseNumber[i];
    }
    //convert the integer to long (more memory)
    long stringInt = atoi(toString.c_str());
    //hash the resulting integer
    int key = hash(stringInt);

    Node* oldNode = &nodes.at(key);
    //if old was null
    if (oldNode == nullptr) {
        Node* newNode = new Node(course, key);
        nodes.insert(nodes.begin() + key, (*newNode));
    }
    //if key was not set
    else if (oldNode->key == UINT_MAX) {
        oldNode->key = key;
        oldNode->course = course;
        oldNode->next = nullptr;
    }
    else {
        while (oldNode->next != nullptr) {
            oldNode = oldNode->next;
        }
        oldNode->next = new Node(course, key);
    }
}

void HashTable::PrintAllCourses() {
    //clear sortedCourses from a previous run
    sortedCourses.clear();
    //iterate through the nodes looking for courses that were added
    for (int i = 0; i < nodes.size(); i++) {
        if (nodes[i].key != UINT_MAX) {
            this->sortedCourses.push_back(nodes[i].course);
        }
    }
    //call quicksort function to sort courses by courseNumber
    quickSort(this->sortedCourses, 0, this->sortedCourses.size()-1);
    //print the resulting sorted vector
    for (int j = 0; j < this->sortedCourses.size(); j++) {
        cout << this->sortedCourses.at(j).getInformation() << endl;
    }
}


Course HashTable::Search(string courseNumber) {
    Course course;
    //using the same hashing as the hash function to get the key
    string toString = "";
    int intString = 0;
    for (int i = 0; i < courseNumber.size(); i++) {
        while (!isdigit(courseNumber[i])) {
            intString = intString + (int(courseNumber[i]));
            i++;
        }
        toString = toString + courseNumber[i];
    }
    long stringInt = atoi(toString.c_str());
    int key = hash(stringInt);

    // if entry found for the key
    Node* searchNode = &nodes.at(key);
    if (searchNode->course.courseNumber == courseNumber) {
        //return node bid
        return searchNode->course;
    }
    if ((searchNode->key == UINT_MAX) || searchNode == nullptr) {
        // if no entry found for the key
            // return bid
        return course;
    }
    if (searchNode != nullptr) {
        // while node not equal to nullptr
        while (searchNode != nullptr) {
            // if the current node matches, return it
            if (searchNode->course.courseNumber == courseNumber) {
                return searchNode->course;
            }
            //node is equal to next node
            searchNode = searchNode->next;
        }
    }
    return course;
}

bool validateData(vector<vector<string>> parsedText, int line) {
    //if there are only 2 fields (no prerequisites)
    if (parsedText[line].size() == 2) {
        return true;
    }
    //verifies that all prerequisites of the course are found in the txt file
    else if (parsedText[line].size() > 2) {
        for (int i = 2; i < parsedText[line].size(); i++) {
            bool tempValid = false;
            for (int j = 0; j < parsedText.size(); j++) {
                if (parsedText[line][i] == parsedText[j][0]) {
                    tempValid = true;
                }
            }
            return tempValid;
        }
    }
    else {
        return false;
    }
    return false;
}

void loadCourses(string filePath, HashTable* hashTable) {
    ifstream file;
    file.open(filePath);
    string line;
    vector<vector<string>> parsedText;
    string tempString = "";
    //sets delimiter as ,
    char delimiter = ',';
    vector<string> tempVector;
    //while there are lines
    while (getline(file, line)) {
        //clear temp/last line
        tempVector.clear();
        //iterate through characters until a , is reached
        for (int i = 0; i < line.size(); i++) {
            if (line[i] != delimiter) {
                tempString = tempString + line[i];
            }
            else {
                //add string to vector
                tempVector.push_back(tempString);
                tempString = "";
            }
        }
        //add vector to 2d vector of parsed output
        tempVector.push_back(tempString);
        parsedText.push_back(tempVector);
        tempString = "";
    }
    //iterate through each parsed line
    for (int j = 0; j < parsedText.size(); j++) {
        //calls validate data function to confirm txt file is valid
        if (validateData(parsedText, j)) {
            Course course;
            course.courseNumber = parsedText[j][0];
            course.courseName = parsedText[j][1];
            //if course has prerequisites
            if (parsedText[j].size() > 2) {
                //set hasPrerequisites member to true
                course.hasPrerequisites = true;
                //add prerequisite classes to vector
                for (int k = 2; k < parsedText[j].size(); k++) {
                    course.prerequisites.push_back(parsedText[j][k]);
                }
            }
            else {
                //if no prerequisites, set hasPrerequisites to false.
                course.hasPrerequisites = false;
            }
            hashTable->Insert(course);
        }
        else {
            cout << "ERROR: txt file not valid" << endl;
        }
    }
}



int main(int argc, char*argv[])
{
    //change this to use a different file
    string filePath = "courseInfo.txt";

    string courseDetails;
    Course tempCourse;
    HashTable* courseTable{};
    int choice = 0;
    while (choice != 9) {
        cout << "Menu:" << endl;
        cout << "  1. Load Data Structure" << endl;
        cout << "  2. Print Course List" << endl;
        cout << "  3. Print Course" << endl;
        cout << "  9. Exit" << endl;
        cout << "What would you like to do? ";
        cin >> choice;

        switch (choice) {
        case 1:
            courseTable = new HashTable();
            loadCourses(filePath, courseTable);
            choice = 0;
            break;
        case 2:
            courseTable->PrintAllCourses();
            choice = 0;
            break;
        case 3:
            cout << "What course do you want to know about? ";
            cin >> courseDetails;
            //checks to see if input is valid
            if ((courseDetails.size() != 7)) {
                cout << "Incorrect format for class." << endl;
            }
            else {
                bool isValid = true;
                //again checks to see if input is valid
                for (int i = 4; i < courseDetails.size(); i++) {
                    if (!isdigit(courseDetails[i])) {
                        isValid = false;
                    }
                }
                //if input is invalid
                if (isValid == false) {
                    cout << "Incorrect format for class." << endl;
                    choice = 0;
                    break;
                }
                //if input is valid
                else {
                    tempCourse = courseTable->Search(courseDetails);
                    if (tempCourse.courseNumber.empty()) {
                        cout << "Course not found." << endl;
                    }
                    else {
                        cout << tempCourse.getInformation() << endl;
                        if (tempCourse.hasPrerequisites)
                            cout << "Prerequisites: ";
                        for (int i = 0; i < tempCourse.prerequisites.size(); i++) {
                            cout << tempCourse.prerequisites[i] << " ";
                        }
                        cout << endl;
                    }
                }
            }
            choice = 0;
            break;

        case 9:
            break;
        default:
            cout << choice << "is not a valid option." << endl;
            choice = 0;
            break;
        }
    }
    cout << "Thank you for using the course planner!";
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
