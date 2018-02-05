
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
void sortRuns();
void sortDep(department** departments, int endIndex);
void sortEmp(employee** employees, int endIndex);
int readEmployees(FILE* fname, employee** dest);
void writeEmployees(FILE* fname, employee** dest);
int readDepartments(FILE* fname, department** dest);
void writeDepartments(FILE* fname, department** dest);
bool compareEmployee(employee const& lhs, employee const& rhs);
bool compareDepartment(department const& lhs, department const& rhs);
void join();
void setUpFiles(FILE** toSetUp, const char* preFname);
void fillMemory(FILE** empFiles, FILE** depFiles, employee** employees, department** departments);
void foundAMatch(FILE* output, FILE* depFile, employee* emp, department* dep);


//Constants
const int NUM_BLOCKS = 22;
const char EMP_FNAME[] = "employee.csv";
const char DEPT_FNAME[] = "department.csv";
const char OUTPUT_FNAME[] = "join.csv";
const char ESORT_NAME[] = "empSort";
const char DSORT_NAME[] = "depSort";

// Main function
int main(){
    sortRuns();
	join();
}

//this function takes a file name and a string of args
//opens up the file and stops the program if the file
//doesn't open.
FILE* openFile(const char* fname,const char* args){
	FILE* fp = fopen(fname, args);
	if(!fp){
		perror("Error opening file");
		exit(1);
	}
}

//takes a list of file pointers and resets them to point at the
//begining of the file.
void setUpFiles(FILE** toSetUp, const char* preFname){
    char pname[20];
    strcpy(pname, preFname);
    char fname[21];
	for(int i = 0; i < 10; ++i){
		 sprintf(fname, "%s%i", pname, i);
         toSetUp[i] = fopen(fname, "r");
		if(!toSetUp[i]){
            cout << i << "\n";
            return;
		}
	}
}

//takes to lists of files that point to the files on the disk
//that contain the sorted runs of employees and departments.
void join(){
	//sets files back to location 0.
    FILE** empFiles = new FILE*[20];
    FILE** depFiles = new FILE*[20];
	setUpFiles(empFiles, ESORT_NAME);
	setUpFiles(depFiles, DSORT_NAME);
	
	//set up list to hold our objects while we preform the merge.
	department** departments = new department*[10];
	employee** employees = new employee*[10];
    for(int i = 0; i < 10; i++){
        departments[i] = NULL;
        employees[i] = NULL;
    }
	empDepartment* toOutput = new empDepartment;
	FILE* output = fopen(OUTPUT_FNAME, "w");
	
	//fill the memory with the smallest values from the files.
	fillMemory(empFiles, depFiles, employees, departments);
    
    bool done = false;
    bool empLowSet = false;
    bool depLowSet = false;
    int empLow = 0;
    int depLow = 0;
    
    while(!done){
        //find the smallest records
         for(int i =0; i < 10; i ++){
             //if we've reached the end of both arrays break out of the loop.
             if(!employees[i]  && !departments[i] && empLowSet && depLowSet){
                 break;
             }
             if(departments[i]) {
                 if(departments[i]->managerId <= departments[depLow]->managerId){
                    depLow = i;
                    depLowSet =true;
                }
            }  else if(!depLowSet){
                depLow++;
            }
            if(employees[i]){
                if(employees[i]->eid <= employees[empLow]->eid){
                    empLow = i;
                    empLowSet = true;
                }
            } else if(!empLowSet) {
                empLow++;
            }
         }
         
         
         cout << "\n\n\n";
         cout << "depIndex:" << depLow << "\n";
         cout << "empIndex:" << empLow << "\n";
         displayDep(departments[depLow]);
         displayEmp(employees[empLow]);
         cout << "\n\n\n";
         
         //check to see if they join  if they join load a new department.
         for(int i = 0; i < 3; i++){
             displayDep(departments[i]);
             displayEmp(employees[i]);
         }
         if(departments[depLow]->managerId == employees[empLow]->eid){
            foundAMatch(output, depFiles[depLow], employees[empLow], departments[depLow]);
             //if the smallest department is > smallest employee throw out the employee and get a new one. and start over.
         } else if(departments[depLow]->managerId > employees[empLow]->eid){ 
            delete employees[empLow];
            employees[empLow] = getEmpTouple(empFiles[empLow]);
         } else if(departments[depLow]->managerId < employees[empLow]->eid){ //if emp > dep trhow out dep and search dep to see if there is a match for emp.
             for(int i = 0; i < 10; i++){
                 //if we reach the end with out finding a match break
                 if(departments[i] == NULL){
                     break;
                 }
                 //if we find a match output it.
                if(employees[empLow]->eid == departments[i]->managerId){
                   foundAMatch(output, depFiles[i], employees[empLow], departments[i]); 
                }
             }
             //get a new department
             departments[depLow] = getDeptTouple(depFiles[depLow]);
         }
         bool empDone = true;
         bool depDone = true;
        for(int i = 0; i < 10; i++){
            if(employees[i]){
                empDone =false;
            }
            
            if(departments[i]){
                depDone = false;
            }
         }
         done = empDone || depDone;
         empLow = 0;
         depLow = 0;
         empLowSet = false;
         depLowSet = false;
    }
    //clean up. close and delete our files free up memory.
    
}

//this function will be called every time we find a touple pair that matches and needs to be output.
//this function will merge the department and employee. Remove the department and load a new one.
//this function will also free the memory for the department before loading a new one and will free the memory
//of out put once the data has been written.
void foundAMatch(FILE* output, FILE* depFile, employee* emp, department* dep){
    empDepartment* toOutput = copy(dep, emp);
    writeEmpDepartment(output, toOutput);
    delete toOutput;
    delete dep;
    dep = getDeptTouple(depFile);
}



//this function takes our empfiles array, department files array and our list of employees and departments
//and fills the lists from the file reading in the smallest tuple from each file.
void fillMemory(FILE** empFiles, FILE** depFiles, employee** employees, department** departments){
		bool empStop = false;
        bool depStop = false;
        bool stop = true;
		int empFileIndex = 0;
		int depFileIndex = 0;
		int eIndex = 0;
		int dIndex = 0;
		while(stop){
		if(empFiles[empFileIndex] != NULL && !empStop){
			employees[eIndex] = getEmpTouple(empFiles[empFileIndex]);
			empFileIndex++;
			eIndex++;
		} else {
			empStop = true;
		}
		if(depFiles[depFileIndex] != NULL && !depStop){
			cout << depFileIndex << "\n";
			departments[dIndex] = getDeptTouple(depFiles[depFileIndex]);
			depFileIndex++;
			dIndex++;
		} else {
			depStop = true;
		}
        stop = !(empStop && depStop);
		if(eIndex + dIndex > NUM_BLOCKS - 2){
			stop = false;
		}
        
	}
}

//opens the files for the employee and department relations. Then takes those files and uses them to read in
//blocks of size m - 1, sorts them on the join attribute and writes them to disk. This function will also calculate
//how many files will be need to give each run its own file.
void sortRuns(){
	FILE* efp = openFile(EMP_FNAME, "r");
    FILE* dfp = openFile(DEPT_FNAME, "r");
    FILE* empFile;
    FILE* depFile;
    
	//reads in 21 employee's at a time and sorts them outputting them to their own file.
	employee** tempEmp = new employee*[NUM_BLOCKS - 1];
	int i = 0;
    int num = 0;
    char fname[21];
	do{ 
        if(i == NUM_BLOCKS - 1){
            perror("Relation is to large for this algorithm. Not enough memory\n");
            exit(0);
        }
		num = readEmployees(efp, tempEmp);
        if(num > 0){
            sortEmp(tempEmp, num + 1);
            strcpy(fname, ESORT_NAME);
            sprintf(fname,"%s%i", fname, i);
            empFile = fopen(fname, "w");
            writeEmployees(empFile, tempEmp);
            fclose(empFile);
        }
        i++;
	} while(num > 0);
	int numEmp = i;
    i = 0;
    num = 0;
    department** tempDep = new department*[NUM_BLOCKS - 1];
    do{
        if((i + numEmp) == (NUM_BLOCKS - 1)){
            perror("Relation to large for this algorithm. Not enough memory\n");
            exit(0);
        }
        num = readDepartments(dfp, tempDep);
        if(num > 0){
			sortDep(tempDep, num + 1);
            strcpy(fname, DSORT_NAME);
            sprintf(fname, "%s%i", fname, i);
            depFile = fopen(fname, "w");
            writeDepartments(depFile, tempDep);
            fclose(depFile);
        }
		i++;
    } while(num > 0);
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
        if(toDisk[i]){
            writeEmployee(fname, toDisk[i]);
			fflush(fname);
        }
	}
}

//writes a run of departments to disk
void writeDepartments(FILE* fname, department** toDisk){
		//don't do any thing if the we don't have a file or any data to output.
	if(!fname || !toDisk){
		return;
	}
	for(int i = 0; i < NUM_BLOCKS - 1; ++i){
        if(toDisk[i]){
            writeDepartment(fname, toDisk[i]);
			fflush(fname);
        }
	}
}

// takes a list of departments and returns a list of departments sorted on
// mangerId.
void sortDep(department** departments, int endIndex){
    std::sort(departments, (departments + endIndex), compareDep);
}


//takes a list of employees and returns a list of employees sorted on eid. 
void sortEmp(employee** employees, int endIndex){
    std::sort(employees, (employees + endIndex), compareEmp);
}


//This function takes a file pointer and an employee pointer and writes that employee out to a file
void writeEmployee(FILE* file, employee* emp)
{
	fprintf(file, "\"%ld\",\"%s\",\"%li\",\"%.2lf\"\n", emp->eid, emp->ename, emp->age, emp->salary); 
}

//this function takes a file pointer and a department pointer and writes that department out to the file.
void writeDepartment(FILE* file, department* dep)
{
	fprintf(file, "\"%ld\",\"%s\",\"%.2lf\",\"%li\"\n", dep->did, dep->dname, dep->budget, dep->managerId); 
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

