
//author: Thomas Noelcke
//
//
//Pourpose: The pourpose of this program is to preform a sorted merge join on the two csv files in this
//Repo. This will be done using optimized sort join. The relations that we will be joining are
//
//Relations:
//Dept (did (integer), dname (string), budget (double), managerid(integer))
//Emp (eid (integer), ename (string), age (integer), salary (double))
//
//Requirements
//Size of data: Int 4 bytes (32 bit), double 8 byes (64 bits), string 40 bytes.
//In this file we are joining Dept with EMP on Dept.managerid = emp.eid.
//each relations are stored on a separte csv file. each tuple is in a line and
//each feild is seperated by a comma. We must assume that the CSV files are in the CWD
//This program must run on linux to test this program use the voltdb1 server at osu.
//URL: voltdb1.eecs.oregonstate.edu
//
//You can compile and run this program using the following commands:
//g++ main.cpp -o main.out
//main.out
//
//
//Assumptions:
//We only have 22 blocks available to merge the CSV files
//Each block fits at most one tuple.
//
//
//

//#includes
#include<iostream>
#include<stdio.h>
#include<string.h>
#include<fstream>
#include<stdlib.h>
#include<algorithm>
using namespace std;

//struct definitions
//sruct constant that will store the length of a character.
const int STRING_LEN = 40;
//this struct represents an employee in our employee relation.

typedef struct employee
{
    long int eid;
    char ename[STRING_LEN];
    long int age;
    double salary;

    bool operator<(const employee& rhs) const
    {
        return (eid < rhs.eid);
    }
    
} employee;

//this struct represents a department in our department relation.
typedef struct department 
{
    long int did;
    char dname[STRING_LEN];
    double budget;
    long int managerId;
    
    bool operator<(const department& rhs) const
    {
        return (managerId < rhs.managerId);
    }
} department;

typedef struct empDepartment 
{
    long int did;
    long int eid;
    char dname[STRING_LEN];
    char ename[STRING_LEN];
    double budget;
    double salary;
	long int age;
    long int managerId;    
} empDepartment;

//Function Headers
employee* getEmpTouple(FILE* file);
department* getDeptTouple(FILE* file);
empDepartment* copy(department* dep, employee* emp);
FILE* openFile(const char* fname,const char* args);
void displayEmp(employee* emp);
void displayDep(department* dep);
void displayDepEmp(empDepartment* ed);
void writeEmployee(FILE* file, employee* emp);
void writeDepartment(FILE* file, department* dep);
void writeEmpDepartment(FILE* file, empDepartment* join);
FILE ** sortRuns(FILE* empFile, FILE* depFile);
void sortDep(department** departments);
void sortEmp(employee** employees);
bool readEmployees(FILE* fname, employee** dest);
void writeEmployees(FILE* fname, employee** dest);
bool readDepartments(FILE* fname, department** dest);
void writeDepartments(FILE* fname, department** dest);
bool compareEmployee(employee const& lhs, employee const& rhs);
bool compareDepartment(department const& lhs, department const& rhs);


//Constants
const int NUM_BLOCKS = 22;
const char EMP_FNAME[] = "employee.csv";
const char DEPT_FNAME[] = "department.csv";
const char OUTPUT_FNAME[] = "join.csv";
const char ESORT_NAME[] = "empSort";
const char DSORT_NAME[] = "depSort";

// Main function
int main(){
	FILE* efp = openFile(EMP_FNAME, "r");
	FILE* dfp = openFile(DEPT_FNAME, "r");
	employee* emp = getEmpTouple(efp);
	displayEmp(emp);
	department* dep = getDeptTouple(dfp);
	displayDep(dep);
	empDepartment* join = copy(dep, emp);
	displayDepEmp(join);
	FILE* jfp = openFile(OUTPUT_FNAME, "w");
	writeEmpDepartment(jfp, join);
}


FILE* openFile(const char* fname,const char* args){
	FILE* fp = fopen(fname, args);
	if(!fp){
		perror("Error opening file");
		exit(1);
	}
}

//opens the files for the employee and department relations. Then takes those files and uses them to read in
//blocks of size m - 1, sorts them on the join attribute and writes them to disk. This function will also calculate
//how many files will be need to give each run its own file.
void sortRuns(FILE** empFiles, FILE** depFiles){
	//ToDo: calculate the number of files I will need.
	FILE* efp = openFile(EMP_FNAME, "r");
    FILE* dfp = openFile(DEPT_FNAME, "r");
    empFiles = new FILE*[NUM_BLOCKS - 2];
    depFiles = new FILE*[NUM_BLOCKS - 2];
    //so i know how many files i have later once I'm done.
    for (int i = 0; i < NUM_BLOCKS - 2; ++i){
        empFiles[i] = NULL;
        depFiles[i] = NULL;
    }
    
	//reads in 21 employee's at a time and sorts them outputting them to their own file.
	employee** tempEmp = new employee*[NUM_BLOCKS - 1];
	int i = 0;
    int num = 0;
    char fname[21];
    fname = "empSorted"
    fname
	do{ 
        if(i == NUM_BLOCKS - 1){
            perror("Relation is to large for this algorithm. Not enough memory\n");
            exit(3);
        }
		num = readEmployees(efp, tempEmp);
        sortEmp(tempEmp);
        
        empFiles[i] = fopen(
        if(num >= 0){
            writeEmployees(empFiles[i], tempEmp);
        }
        i++;
        
	} while(num >= 0);
	
    i = 0;
    department** tempDep = new department*[NUM_BLOCKS - 1];
    do{
        
    } while(tempDep);
	return NULL;
}


//reads in a list of employees just the right size to fit into the input buffer size - 1
//so we have room to sort the data with a temp slot. Will store the result in dest.
//returns true if there are still more employees to read in returns false if we hit EOF.
int readEmployees(FILE* fname, employee** dest){
	int toReturn = 0;
    if(!fname){
		return -1;
	}
	if(!dest){
		dest = new employee* [NUM_BLOCKS];
	}
	for(int i = 0; i < NUM_BLOCKS - 1; ++i){
		dest[i] = getEmpTouple(fname);
		//tells the calling client we have reached EOF
		if(dest[i]){
			toReturn = i;
		}
	}
	return toReturn;
}


//does the same thing as readEmployees except for departments.
int readDepartments(FILE* fname, department** dest){
	int toReturn = 0;
	if(!fname){
		return -1;
	}
	if(!dest){
		dest = new department* [NUM_BLOCKS];
	}
    
	for(int i = 0; i < NUM_BLOCKS - 1; ++i){
		dest[i] = getDeptTouple(fname);
		//tells the calling client that we have reached eof.
		if(dest[i]){
			toReturn = i;
		}
	}
	return toReturn;
}

//writes a run of employees out to the disk.
void writeEmployees(FILE* fname, employee** toDisk){
	//don't do any thing if the we don't have a file or any data to output.
	if(!fname || !toDisk){
		return;
	}
	for(int i = 0; i < NUM_BLOCKS - 1; ++i){
		writeEmployee(fname, toDisk[i]);
	}
}

//writes a run of departments to disk
void writeDepartments(FILE* fname, department** toDisk){
		//don't do any thing if the we don't have a file or any data to output.
	if(!fname || !toDisk){
		return;
	}
	for(int i = 0; i < NUM_BLOCKS - 1; ++i){
		writeDepartment(fname, toDisk[i]);
	}
}

// takes a list of departments and returns a list of departments sorted on
// mangerId.
//TODO: Implement quick sort. :(
void sortDep(department** departments){
    std::sort(departments, departments + (NUM_BLOCKS - 1));
}


//takes a list of employees and returns a list of employees sorted on eid.
//TODO: Implement quick sort. :(  
void sortEmp(employee** employees){
    std::sort(employees, employees + (NUM_BLOCKS - 1));
}


//This function takes a file pointer and an employee pointer and writes that employee out to a file
void writeEmployee(FILE* file, employee* emp)
{
	fprintf(file, "\"%ld\",\"%s\",\"%li\",\"%.2la\"\n", emp->eid, emp->ename, emp->age, emp->salary); 
}

//this function takes a file pointer and a department pointer and writes that department out to the file.
void writeDepartment(FILE* file, department* dep)
{
	fprintf(file, "\"%ld\",\"%s\",\"%la\",\"%li\"\n", dep->did, dep->dname, dep->budget, dep->managerId); 
}

//this function takes a file pointer and an merged empDepartment pointer
//and writes this out to the file.
void writeEmpDepartment(FILE* file, empDepartment* join)
{
	fprintf(file,"\"%ld\",\"%ld\",\"%s\",\"%s\",\"%.2lf\",\"%.2lf\",\%li\",\"%li\"\n", join->did, join->eid, join->dname, join->ename, join->budget, join->salary, join->managerId, join->age);	
}

//this function takes a file pointer and returns a pointer to an employee object.
employee* getEmpTouple(FILE* file)
{
	employee* emp = NULL;
	if(!feof(file)){
		emp = new employee;
		fscanf(file, "\"%ld\",\"%[^\"]\",\"%ld\",\"%la\"\n", &emp->eid, emp->ename, &emp->age, &emp->salary);
	} else {
		emp = NULL;
	}
	return emp;
}

//this function takes a file point and returns a pointer to a department object.
department* getDeptTouple(FILE* file)
{	
	department* dep = NULL;
	if(!feof(file)){
		dep = new department;
		fscanf(file, "\"%ld\",\"%[^\"]\",\"%la\",\"%ld\"\n", &dep->did, dep->dname, &dep->budget, &dep->managerId);
	}
    return dep;
}


//this function takes a department pointer and an employee pointer
//and creates a new empDepartment by doing a deep copy of dep and emp.
empDepartment* copy(department* dep, employee* emp)
{   
    //checks if the function arguments are null.
    if(!dep || !emp){
        return NULL;
    }
    //copy the data over to the new struct.
    empDepartment* copy = new empDepartment;
    copy->did = dep->did;
    copy->eid = emp->eid;
    strcpy(copy->dname, dep->dname);
    strcpy(copy->ename, emp->ename);
    copy->budget = dep->budget;
    copy->salary = emp->salary;
    copy->managerId = dep->managerId;
	copy->age = emp-> age;

    return copy;
}

//Takes a department pointer and displays that department to standard out.
void displayDep(department* dep){
	if(dep){
		cout<<"did: "<<dep->did<<" name: " << dep->dname << " budget: " << dep->budget << " Managerid: " << dep->managerId << "\n";
	} else {
		cout << "No Department to display\n";
	}
}

//Takes an employee pointer and displays the employee to stdout.
void displayEmp(employee* emp){
	if(emp){
		cout<<"eid: "<< emp->eid <<" name: " << emp->ename << " age: " << emp->age << " salary: " << emp->salary << "\n";
	} else{
		cout << "No Employee to display\n";
	}
}

void displayDepEmp(empDepartment* ed){
	if(ed){
		cout<<"eid: "<< ed->eid <<" name: " << ed->ename << " age: " << ed->age << " salary: " << ed->salary << "\n";
		cout<<"did: "<<ed->did<<" name: " << ed->dname << " budget: " << ed->budget << " Managerid: " << ed->managerId << "\n";
	} else {
		cout << "Nothing to display\n";
	}
}

