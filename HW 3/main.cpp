
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
//We only have 22 blocks availible to merge the CSV files
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
using namespace std;

//struct definitions
//sruct constant that will store the length of a character.
const int STRING_LEN = 40;
//this struct represents an employee in our employee relation.
typedef struct employee
{
    int eid;
    char ename[STRING_LEN];
    int age;
    double salary;
    
} employee;

//this struct represents a department in our department relation.
typedef struct department 
{
    int did;
    char dname[STRING_LEN];
    double budget;
    int managerId;
} department;

typedef struct empDepartment 
{
    int did;
    int eid;
    char dname[STRING_LEN];
    char ename[STRING_LEN];
    double budget;
    double salary;
    int managerId;
    
} empDepartment;

//Function Headers
employee* getEmpTouple(FILE* file);
department* getDeptTouple(ofstream file);
empDepartment* copy(department * dep, employee* emp);
FILE* openFile(char* fname, char* args);

//Constants
const int NUM_BLOCKS = 22;
const char EMP_FNAME[] = "employee.csv";
const char DEPT_FNAME[] = "department.csv";



int main(){
	FILE* fp;
	fp = fopen(EMP_FNAME, "r");
	if(!fp){
		perror("Error opening file");
		exit(2);
	}
	employee* emp = NULL;
	emp = getEmpTouple(fp);
	
}


FILE* openFile(char* fname, char* args){
	FILE* fp = fopen(fname, args);
	if(!fp){
		perror("Error opening file");
		exit(1);
	}
}

//this function takes a file pointer and retunrs a pointer to an employee object.
employee* getEmpTouple(FILE* file)
{
	
    return NULL;
}


//this function takes a file point and returns a pointer to a department object.
department* getDeptTouple(FILE* fp)
{
    return NULL;
}


//this function takes a department pointer and an employee pointer
//and creates a new empDepartment by doing a deep copy of dep and emp.
empDepartment* copy(department* dep, employee* emp)
{   
    //checks if the function arguments are null.
    if(!dep || !emp){
        return NULL;
    }
    //copy the data over to the new scrut.
    empDepartment* copy = new empDepartment;
    copy->did = dep->did;
    copy->eid = emp->eid;
    strcpy(copy->dname, dep->dname);
    strcpy(copy->ename, emp->ename);
    copy->budget = dep->budget;
    copy->salary = emp->salary;
    copy->managerId = dep->managerId;

    return copy;
}



