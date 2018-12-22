
/* CSci4061 F2018 Assignment 1
* login: CSE KELLER 4250 LAB MACHINES
* date: 10/03/2018
* name: Guangyu Yan, Huilin Zhang, Ziqian Qiu 
* yanxx630, zhan5992, qiuxx283*/


Who did what on the program
We did all the algorithms and most of our coding work during our meeting

* Guangyu Yan
    *Guangyu finished the warm up,Readme file and contributes ideas of the whole framework algorithms design.

* Ziqian Qiu 
    *Ziqian finished the program debugging and testing of our program. She also did the comment of the projects


* Huilin Zhang
    *Huilin did most of the coding work during our meeting and provided good ideas of debugging.

Porgram Syntax

*General usage
  ./make4061 [options] [target]
  *There could be multiple options combined together to use given a single target

Types of executions

1. ./make4061: This will build the first target found in makefile
2. ./make4061 specificTarget: This will build only the specific target
3. ./make4061 -f yourownmakefile


Types of Options
  1. -f filename: filename will be the name of the makefile, otherwise the default name “Makefile” is assumed.

  2. specificTarget: specificTarget will be the name of any single target in the Makefile.

  3. -h: print usage info.


Test cases:
We have tested all the cases we have given and we also make our own testcase as follows.
1. Missing makefile
2. Missing file
3. Missing & Wrong dependencies
4. Wrong input dependencies
5. After the first make, we delete some files, and test whether the make will update the changes.
6. Other tests to make sure our program has the same output as using make

