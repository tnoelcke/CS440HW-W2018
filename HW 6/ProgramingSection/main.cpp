
//author: Thomas Noelcke
//
//
//Pourpose: The pourpose of this program is to preform a sorted merge join on the two csv files in this
//Repo. This will be done using merge sort join. However the files are already in sorted order so we will
//only be doing the merge phase of the merge sort join. In this join however the join will need to be crash
//safe.
//
//Relations:
//Dept (did (integer), dname (string), budget (double), managerid(integer))
//Emp (eid (integer), ename (string), age (integer), salary (double))
//
//  This Merge must preform a Safe Merge which will once interupted start 
//  Where it left off and finish the merge. This program will be able to do this no matter
//  how many times its interupted or when it is interupted.
//
//Assumptions:
//  We only have 22 blocks available to merge the CSV files
//  Each block fits at most one tuple.
//  Files are in sorted order and you only need to read from two files.
//
//

//#includes
#include<iostream>
#include<stdio.h>
#include<string.h>
#include<string>
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
    
} employee;

//this struct represents a department in our department relation.
typedef struct department 
{
	long int managerId;
    long int did;
    char dname[STRING_LEN];
    double budget;
   
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

bool compareEmp(employee* lhs, employee* rhs){ return lhs->eid < rhs->eid; }
bool compareDep(department* lhs, department* rhs) {return lhs->managerId < rhs->managerId;}

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
void foundAMatch(FILE* output, FILE* depFile, department* dep, employee* emp);
void freeEmp(employee* emp);
void freeDep(department* dep);
void join(FILE* output, FILE* depFile, FILE* empFile);
void sortDep();


//Constants
const int NUM_BLOCKS = 22;
const char EMP_FNAME[] = "employee.csv";
const char DEPT_FNAME[] = "department.csv";
const char OUTPUT_FNAME[] = "join.csv";
const char LOG_FNAME[] = "joinLog";

// Main function
int main(){
    FILE* depFile = openFile(DEPT_FNAME, "r");
    FILE* empFile = openFile(EMP_FNAME, "r");
    FILE* outFile = openFile(OUTPUT_FNAME, "w");
    join(outFile, depFile, empFile);
    fclose(depFile);
    fclose(empFile);
    fclose(outFile);
    exit(0);
}

//this function preforms the join by reading in one tuple from each file
//and attempting the join. IN the future this function will also be responsible for
//preforming actions to log all steps.
void join(FILE* output, FILE* depFile, FILE* empFile){
    //set up variables to hold read in values
    employee* emp;
    department* dep;
    //this value will get set to true if we hit EOF on either file.
    emp = getEmpTouple(empFile);
    dep = getDeptTouple(depFile);
    bool isDone = emp == NULL || dep == NULL;
    do {
        displayDep(dep);
        displayEmp(emp);
        if(emp != NULL && dep != NULL){
            //if the ID's match join the two and out put them
            if(emp->eid == dep->managerId){
                cout << "FOUND A MATCH\n";
                empDepartment* toOutput = copy(dep, emp);
                writeEmpDepartment(output, toOutput);
                delete toOutput;
                delete dep;
                dep = getDeptTouple(depFile);
            //if emp is > dep discard of dep and read in a new emp
            } else if(emp->eid > dep->managerId){
                cout << "EMP > DEP \n";
                delete dep;
                dep = getDeptTouple(depFile);
             //if dep > emp discard emp and read in a new emp.
            } else {
                cout << "DEP > EMP\n";
                delete emp;
                emp = getEmpTouple(empFile);
            }
        }
        //if either emp or dep are set to null we have hit EOF
        isDone = emp == NULL || dep == NULL;
    } while(!isDone);
}

//reads in and sorts the DEP file then writes the result to the file. made this so
//I could sort the files i was given in the given format. RUN THIS FUNCTION TO SORT THE
//department.csv file so that it is in accending order on mangerid.
void sortDep(){
    FILE* depFile = openFile(DEPT_FNAME, "r");
    department** toSort = new department*[100];
    for(int j = 0; j < 100; j++){
        toSort[j] = NULL;
    }
    department* toAdd;
    toAdd = getDeptTouple(depFile);
    int i = 0;
    while(toAdd != NULL)
    {
        toSort[i] = toAdd;
        i++;
        toAdd = getDeptTouple(depFile);
        displayDep(toAdd);
    }
    std::sort(toSort, toSort + i, compareDep);
    fclose(depFile);
    depFile = openFile(DEPT_FNAME, "w");
    for(int j = 0; j < i; j++){
        writeDepartment(depFile ,toSort[j]);
    }
    fclose(depFile);
}


//this function takes a file name and a string of args
//opens up the file and stops the program if the file
//doesn't open. Takes either r or w for args.
FILE* openFile(const char* fname,const char* args){
	FILE* fp = fopen(fname, args);
	if(!fp){
		perror("Error opening file");
		exit(1);
	}
}

//frees the memory for a single employee
void freeEmp(employee* emp){
  if(emp != NULL){
    delete emp;
    emp = NULL;
  }
}

//frees the memeory for a single department.
void freeDep(department* dep){
  if(dep != NULL){
    delete dep;
    dep = NULL;
  }
}


//This function takes a file pointer and an employee pointer and writes that employee out to a file
void writeEmployee(FILE* file, employee* emp)
{
	fprintf(file, "\"%ld\",\"%s\",\"%li\",\"%.2lf\"\n", emp->eid, emp->ename, emp->age, emp->salary); 
    fflush(file);
}

//this function takes a file pointer and a department pointer and writes that department out to the file.
void writeDepartment(FILE* file, department* dep)
{
	fprintf(file, "\"%ld\",\"%s\",\"%.2lf\",\"%li\"\n", dep->did, dep->dname, dep->budget, dep->managerId);
    fflush(file); 
}

//this function takes a file pointer and an merged empDepartment pointer
//and writes this out to the file.
void writeEmpDepartment(FILE* file, empDepartment* join)
{
	fprintf(file,"\"%ld\",\"%ld\",\"%s\",\"%s\",\"%.2lf\",\"%.2lf\",\%li\",\"%li\"\n", join->did, join->eid, join->dname, join->ename, join->budget, join->salary, join->managerId, join->age);	
    fflush(file);
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

//Takes and employee pointer and displays the Employee out to stdout.
//Used for debugging.
void displayDepEmp(empDepartment* ed){
	if(ed){
		cout<<"eid: "<< ed->eid <<" name: " << ed->ename << " age: " << ed->age << " salary: " << ed->salary << "\n";
		cout<<"did: "<<ed->did<<" name: " << ed->dname << " budget: " << ed->budget << " Managerid: " << ed->managerId << "\n";
	} else {
		cout << "Nothing to display\n";
	}
}

