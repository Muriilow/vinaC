#ifndef VINAC_H
#define VINAC_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#define NAME_SIZE 1024

/* The struct that holds the information about the archive */
struct Member
{
    char name[NAME_SIZE];
    uid_t UID;
    int pos;
    unsigned int origSize;
    unsigned int size;
    unsigned int order;
    time_t modifData;
};

/* The struct that holds the members count */
struct Directory 
{
    char name;
    int quantity;
};

/*Printing function that explains what the program does and how to use it*/
void ExplainProg();

/*
 * Insert the archive content into the binary and it's information.
 * archive: The file that is going to have it's content saved;
 * binary: The file that is going to save the content of archive;
 * name: String that has the name of the archive;
*/
void InsertNormalArchive(FILE* archive, FILE* binary, char* name);

/*
 * Insert the archive content into the binary in a compressed form.
 * archive: The file that is going to have it's content saved;
 * binary: The file that is going to save the content of archive;
 * name: String that has the name of the archive;
*/
void InsertCompressedArchive(FILE* archive, FILE* binary, char* name);

/*
 * List all the members with its respective informations.
 * binary: The file that is going to be listed;
*/
void ListMembers(FILE* binary);

/*
 * Extract all the archives saved on the binary.
 * binary: The file that have all the members information and content;
*/
void ExtractAllArchives(FILE* binary);

/*
 * Extract the specified archive saved on the binary.
 * member: The specific member/archive we want to extract;
 * binary: The file that have the member information and content;
*/
void ExtractArchive(char* name, FILE* binary);

/*
 * Move the content and struct position of member1,
 * If no more members is specified, member1 will be moved to the start of the binary.
 * If another member is specified, member1 will be moved after member2.
 * name1: The member that will be moved;
 * name2: The member that serve as reference to move the member1;
 * binary: The binary file that have the information needed;
*/
void MoveMembers(char* name1, char* name2, FILE* binary);

/*
 * Remove the content and struct of the specified member.
 * name: The name of the member to be removed;
 * binary: The binary file that have the information needed;
*/
void RemoveMember(char* name, FILE* binary);
#endif
