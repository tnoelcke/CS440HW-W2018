
//author: Thomas Noelcke
//Class: CS 440 Winter 2018
//Assignment: HW6
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
#include<sys/stat.h>
#include<ctime>
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

typedef struct logEntry{
    int empPos;
    int depPos;
    int joinPos;
} logEntry;

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
void join(FILE* output, FILE* depFile, FILE* empFile, FILE* logFile);
void sortDep();
void writeLog(FILE* logFile, logEntry* entry);
logEntry* readLog(FILE* Log);
void displayLog(logEntry* toDisplay);
void recover(FILE* logFile, FILE* depFile, FILE* empFile, FILE* output);
void outPutLog(FILE* logFile, long int dep, long int emp, long int output);
logEntry* getLastEntry(FILE* logFile);
bool fileExits(const char* fname);
void testCrash();
void cleanUp();

//Constants
const int NUM_BLOCKS = 22;
const char EMP_FNAME[] = "Emp.csv";
const char DEPT_FNAME[] = "Dept.csv";
const char OUTPUT_FNAME[] = "join.csv";
const char LOG_FNAME[] = "joinLog";
const bool TEST_CRASH = true;
const int STABILITY = 33;

// Main function
// First we set up the files we know we are going to need to read form.
// Then we check if there is a log file from a previously aborted session.
// If yes the we prefmore a recovery to recover the staet of the aborted merge.
// IF no we start a normal merge.
int main(){
    srand(time(0));
    FILE* depFile = openFile(DEPT_FNAME, "r");
    FILE* empFile = openFile(EMP_FNAME, "r");
    FILE* outFile;
    FILE* logFile;
    if(fileExits(LOG_FNAME) && fileExits(OUTPUT_FNAME)){
        cout << "State Dirty\nRecovering Aborted Merge\n";
        logFile = openFile(LOG_FNAME, "a+");
        outFile = openFile(OUTPUT_FNAME, "a");
        recover(logFile, depFile, empFile, outFile);
        cout << "Merge Recovered restarting Join\n";
    } else {
        cout << "State Clean\nPreforming Merge\n";
        outFile = openFile(OUTPUT_FNAME, "w");
        logFile = openFile(LOG_FNAME, "w"); 
    }
    join(outFile, depFile, empFile, logFile);
    cout << "Join Complete\nRemoving Log\n";
    fclose(depFile);
    fclose(empFile);
    fclose(outFile);
    fclose(logFile);
    cleanUp();
    exit(0);

}

//checks if a file exits and is non empty.
//takes a file name and returns weather or not that file exists and is nonempty.
bool fileExits(const char* fname) {
    bool result;
    FILE* fp = fopen(fname, "r");
    result = (fp != NULL);
    if(fp != NULL) {
        result = fgetc(fp) != EOF;
        fclose(fp);
    }
    return result;
}

//delets the logfile after we are done using it as to show that the join was sucessfully completed.
void cleanUp(){
    if(remove(LOG_FNAME) != 0){
        cout << "There was an error deleting the log file\n";
    } else {
        cout << "Log File sucessfully removed\n";
    }
}


//this function preforms the join by reading in one tuple from each file
//and attempting the join. IN the future this function will also be responsible for
//preforming actions to log all steps.
void join(FILE* output, FILE* depFile, FILE* empFile, FILE* logFile){
    //set up variables to hold read in values
    employee* emp;
    department* dep;
    //this value will get set to true if we hit EOF on either file.
    emp = getEmpTouple(empFile);
    dep = getDeptTouple(depFile);
    int empPrevious = 0;
    int depPrevious = 0;
    bool isDone = emp == NULL || dep == NULL;

    //will replace these later with the positions of the files when we start.
    outPutLog(logFile, ftell(depFile), ftell(empFile), ftell(output));
    do {
        if(emp != NULL && dep != NULL){
            //if the ID's match join the two and out put them
            if(emp->eid == dep->managerId){
                testCrash();
                empDepartment* toOutput = copy(dep, emp);
                writeEmpDepartment(output, toOutput);
                outPutLog(logFile, depPrevious, empPrevious, ftell(output));
                delete toOutput;
                delete dep;
                depPrevious = ftell(depFile);
                dep = getDeptTouple(depFile);
                outPutLog(logFile, depPrevious, empPrevious, ftell(output));

            //if emp is > dep discard of dep and read in a new emp
            } else if(emp->eid > dep->managerId){
                delete dep;
                depPrevious = ftell(depFile);
                dep = getDeptTouple(depFile);
                outPutLog(logFile, depPrevious, empPrevious, ftell(output));
                testCrash();
             //if dep > emp discard emp and read in a new emp.
            } else {
                delete emp;
                empPrevious = ftell(empFile);
                emp = getEmpTouple(empFile);
                outPutLog(logFile, depPrevious, empPrevious, ftell(output));
                testCrash();
            }
        }
        //if either emp or dep are set to null we have hit EOF
        isDone = emp == NULL || dep == NULL;
    } while(!isDone);
}

//this function takes a FILE pointer and long ints for the position of the department file,
//employee file and the join file, creates a logEntry object and outputs to the log file.
void outPutLog(FILE* logFile, long int dep, long int emp, long int output){
    if(!logFile){
        return;
    }
    logEntry entry;
    entry.depPos = dep;
    entry.empPos = emp;
    entry.joinPos = output;
    writeLog(logFile, &entry);
}

//this file will read the log file and determine what position the 
//depFile and empFile need to be set to. It will then set those files at
//the last known position in the log file.
void recover(FILE* logFile, FILE* depFile, FILE* empFile, FILE* output){
    logEntry* state = getLastEntry(logFile);
    fseek(depFile, state->depPos, SEEK_SET);
    fseek(empFile, state->empPos, SEEK_SET);
    fseek(output, state->joinPos, SEEK_SET);

    //cleaning up memory
    delete state;
}


//reads in the last entry from the logfile. by searching back through the log file and finding
//the second newline. Then reading in the resulting logEntry.
logEntry* getLastEntry(FILE* logFile){
    fseek(logFile, 0, SEEK_END);
    int pos = ftell(logFile);
    int linNum = 0;
    fseek(logFile, pos, SEEK_SET);
    cout << "Retriving Last State from log\n";
    int output;
    char character;
    while(linNum < 2){
        output = fgetc(logFile);
        character = output;
        if(character == '\n'){
            linNum++;
        }
        pos = pos - 1;
        fseek(logFile, pos, SEEK_SET);
    }
    fseek(logFile, pos + 2, SEEK_SET);
    logEntry* last = readLog(logFile);
    cout << "Previous State:\n";
    displayLog(last);
    return last;
}

//this function will randomly crash our program. This is used to check that our program is indeed
//crash resistant.
void testCrash(){
    if(TEST_CRASH && rand() % STABILITY == 0){
        cout << "\nError: Aborting Execution!\n";
        abort();
    }
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

// This function takes a logEntry pointer and a File pointer and writes the log
// entry to the log file.
void writeLog(FILE* log, logEntry* entry){
    if(!log){
        return;
    } else {
        fprintf(log, "%i, %i, %i\n", entry->depPos, entry->empPos, entry->joinPos);
    }
}

//this function takes a FILE* and reads a log entry from that FILE* and
//returns it.
logEntry* readLog(FILE* logFile){
    logEntry* entry = new logEntry;
    if(!feof(logFile)) {
        fscanf(logFile, "%i, %i, %i\n", &entry->depPos, &entry->empPos, &entry->joinPos);
    } else {
        entry = NULL;
    }
    return entry;
}

// This function displays a log entry out to the console.
// This is used for debugging only.
void displayLog(logEntry* toDisplay){
    cout << "DepPos: " << toDisplay->depPos << " EmpPos: " << toDisplay->empPos << " joinPos: " << toDisplay->joinPos << "\n";
}
