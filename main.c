/* CSci4061 F2018 Assignment 1
* login: CSE LAB MACHINES  KELLER 4250
* date: 10/04/2018
* name: Guangyu Yan, Huilin Zhang, Ziqian Qiu
* yanxx630, zhan5992, qiuxx283*/

// This is the main file for the code
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "util.h"

/*-------------------------------------------------------HELPER FUNCTIONS PROTOTYPES---------------------------------*/
void show_error_message(char * ExecName);
//Write your functions prototypes here
void show_targets(target_t targets[], int nTargetCount);


int build(target_t targets[], int index, int nTargetCount);


/*-------------------------------------------------------END OF HELPER FUNCTIONS PROTOTYPES--------------------------*/


/*-------------------------------------------------------HELPER FUNCTIONS--------------------------------------------*/

//This is the function for writing an error to the stream
//It prints the same message in all the cases
void show_error_message(char * ExecName)
{
	fprintf(stderr, "Usage: %s [options] [target] : only single target is allowed.\n", ExecName);
	fprintf(stderr, "-f FILE\t\tRead FILE as a makefile.\n");
	fprintf(stderr, "-h\t\tPrint this message and exit.\n");
	exit(0);
}

//Write your functions here

//Phase1: Warmup phase for parsing the structure here. Do it as per the PDF (Writeup)
void show_targets(target_t targets[], int nTargetCount){
  for (int i = 0; i < nTargetCount; i++){
      printf("TargetName: %s\nDependencyCount: %d\nDependencyNames: %s\nCommand: %s ",
			 targets[i].TargetName,targets[i].DependencyCount,targets[i].DependencyNames[0],targets[i].Command);
       for(int j = 0; j < targets[i].DependencyCount; j++){
            printf("%s ", targets[i].DependencyNames[j]);
        }//end inner
        printf("\n\n");
    }//end for
}



//Phase2: Main Functional "build" function
//This is the function for execute to cmd in makefile via DFS
int build(target_t targets[], int index, int nTargetCount){

//GEBUG:	printf("tar=%s\n",targets[index].TargetName);

	//check target current status first
	if (targets[index].Status==FINISHED){
		return FINISHED;
	}
	if (targets[index].Status==UTD){
		return UTD;
	}


	int i = 0;
	int depindex = 0;
	int Stat = 0;
	//build dependency

	for(;i<targets[index].DependencyCount;i+=1){
//GEBUG:		printf("tar=%s\n",targets[index].TargetName);

		depindex = find_target(targets[index].DependencyNames[i], targets, nTargetCount);
//GEBUG:		printf("dep name=%s dex=%d \n",targets[index].DependencyNames[i] ,depindex );

		if ( depindex == -1) {//dep is leaf
			if (does_file_exist(targets[index].DependencyNames[i])==-1){//missing base file
				printf("make4061: *** No rule to make target '%s', needed by '%s'.  Stop.\n",targets[index].DependencyNames[i],targets[index].TargetName);
				exit(-1);
			}//end check EXIST
		}//end leaf
		else{//not leaf
			Stat+=build(targets, depindex, nTargetCount);
		}//end else
		int comptime=compare_modification_time(targets[index].TargetName, targets[index].DependencyNames[i]);
		if (comptime==1||comptime==0){//same or no need
			Stat+=UTD;
		}
		else if (comptime==-1||comptime==2){//no tar yet or need to rebuild
			Stat+=FINISHED;
		}
//GEBUG:	printf("end for tar=%s\n",targets[index].TargetName );
	}//end for

	//execute the cmd
	if (targets[index].DependencyCount==0||Stat>0){
		printf("%s\n",targets[index].Command);//print out cmd
		targets[index].Status==FINISHED;

		  pid_t pid = fork();
			if (pid == 0) {
					// child process
					char *tokens[ARG_MAX];
					parse_into_tokens(targets[index].Command, tokens, " ");
					execvp(tokens[0], tokens);
					perror("Exec failed");
					exit(-1);
			}

			if (pid > 0) {
					// parent process
					// This part of code is given by the pdf
					int wstatus;
					wait(&wstatus);
					if (WEXITSTATUS(wstatus) != 0) {
							printf("child exited with error code=%d\n", WEXITSTATUS(wstatus));
							exit(-1);
					}
			}

			if (pid < 0) {
					perror("Fail to fork");
					exit(-1);
			}

		return FINISHED;
	}
	else{//if not executed or exit, means up to date
		targets[index].Status==UTD;
		return UTD;
	}
}

/*-------------------------------------------END BUILD---------------------------------------*/


/*-------------------------------------------------------END OF HELPER FUNCTIONS-------------------------------------*/


/*-------------------------------------------------------MAIN PROGRAM------------------------------------------------*/
//Main commencement
int main(int argc, char *argv[])
{
  target_t targets[MAX_NODES];
  int nTargetCount = 0;

  /* Variables you'll want to use */
  char Makefile[64] = "Makefile";
  char TargetName[64];

  /* Declarations for getopt */
  extern int optind;
  extern char * optarg;
  int ch;
  char *format = "f:h";
  char *temp;

  //Getopt function is used to access the command line arguments. However there can be arguments which may or may not need the parameters after the command
  //Example -f <filename> needs a finename, and therefore we need to give a colon after that sort of argument
  //Ex. f: for h there won't be any argument hence we are not going to do the same for h, hence "f:h"
  while((ch = getopt(argc, argv, format)) != -1)
  {
	  switch(ch)
	  {
	  	  case 'f':
	  		  temp = strdup(optarg);
	  		  strcpy(Makefile, temp);  // here the strdup returns a string and that is later copied using the strcpy
	  		  free(temp);	//need to manually free the pointer
	  		  break;

	  	  case 'h':
	  	  default:
	  		  show_error_message(argv[0]);
	  		  exit(1);
	  }

  }

  argc -= optind;
  if(argc > 1)   //Means that we are giving more than 1 target which is not accepted
  {
	  show_error_message(argv[0]);
	  return -1;   //This line is not needed
  }

  /* Init Targets */
  memset(targets, 0, sizeof(targets));   //initialize all the nodes first, just to avoid the valgrind checks

  /* Parse graph file or die, This is the main function to perform the toplogical sort and hence populate the structure */
  if((nTargetCount = parse(Makefile, targets)) == -1)  //here the parser returns the starting address of the array of the structure. Here we gave the makefile and then it just does the parsing of the makefile and then it has created array of the nodes
	  return -1;

  //Phase1: Warmup-----------------------------------------------------------------------------------------------------
  //Parse the structure elements and print them as mentioned in the Project Writeup
  /* Comment out the following line before Phase2 */
  //show_targets(targets, nTargetCount);
  //End of Warmup------------------------------------------------------------------------------------------------------

  /*
   * Set Targetname
   * If target is not set, set it to default (first target from makefile)
   */
  if(argc == 1)
	strcpy(TargetName, argv[optind]);    // here we have the given target, this acts as a method to begin the building
  else
  	strcpy(TargetName, targets[0].TargetName);  // default part is the first target

  /*
   * Now, the file has been parsed and the targets have been named.
   * You'll now want to check all dependencies (whether they are
   * available targets or files) and then execute the target that
   * was specified on the command line, along with their dependencies,
   * etc. Else if no target is mentioned then build the first target
   * found in Makefile.
   */

  //Phase2: Begins ----------------------------------------------------------------------------------------------------
  /*Your code begins here*/

	//find the target's index first
	//return if the target can not be find in the makefile
	int index = find_target(TargetName, targets, nTargetCount);
	if ( index == -1){
		printf("make4061: *** No rule to make target '%s'.  Stop.\n",TargetName);
		return 1;
	}

	//initialize all targets'Status as UNFINISHED
	for (int i = 0; i < nTargetCount; i++){
		targets[i].Status = UNFINISHED;
	}

	int upd = build(targets, index, nTargetCount);
	if (upd > 0){//Status updated
	 //do nothing
	}
	else{//no up date
	 printf ("make4061: '%s' is up to date.\n", TargetName);
	}

  /*End of your code*/
  //End of Phase2------------------------------------------------------------------------------------------------------

  return 0;
}
/*-------------------------------------------------------END OF MAIN PROGRAM------------------------------------------*/
