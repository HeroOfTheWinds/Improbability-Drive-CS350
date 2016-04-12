//CS 350 - Processes Group - "CTRL+ALT+BELIEVE"
// improbabilitydrive.c - The Improbability Drive
// This program behaves in a very improbable way.

#include <errno.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

// Create a linked list struct
typedef struct LNode LNode;
struct LNode
{
    char *fname;
    LNode *next;
};

// Define a function to restart wait() whenever interrupted
pid_t r_wait(int *stat_loc)
{
    int retval;

    // Loop and call wait() whenever an interrupt error occurs
    while (((retval = wait(stat_loc)) == -1) && (errno == EINTR)) ;
    return retval;
}

// Function to construct an array of all files in the directory
// Returns number of items in the array
int makeFileList(LNode *dirlist)
{
    // Special thanks to stackoverflow.com for info on how to get file names
    // Create a new directory object (using dirent.h)
    DIR *d;
    // Make a dirent struct
    struct dirent *dir;
    // Try opening the current directory
    d = opendir(".");
    // Create an index variable for incrementing array indices
    int i = 0;
    
    // Make copy of the pointer to the list for preservation
    LNode *dp = dirlist;

    // If directory opened successfully:
    if (d)
    {
        // While anything to read remains
        while ((dir = readdir(d)) != NULL)
        {
            // Add to list of files, and increment index
            LNode *newN = malloc(sizeof(LNode));
            newN->fname = dir->d_name;
            dp->next = malloc(sizeof(LNode)); 
            dp = newN;
            i += 1;
        }
        dp->next = NULL;
        // No need to leave open.
        closedir(d);
    }
    return i;
}

// Function to take an array of file names and separate out *.sh files
int getSHFiles(LNode *fileList, int n, LNode *shList)
{
    // Declare int to hold total number of hits
    int t = 0;

    // Copy the pointers to avoid overwrite
    LNode *fp = fileList;
    printf("%p\n",(void*)fp);
    LNode *sp = shList;

    // Loop over every item in the file list
    int i;
    for (i = 0; i < n; i++)
    {
        printf("%s\n","Before printname");
        printf("%s\n",fp->fname);
        puts("before string right char");
        // Test if it has the .sh extention
        char *dot = strrchr(fp->fname,'.');
        puts("after strrchar");
        if (dot && !strcmp(dot, ".sh"))
        {
            puts("before sp fname assign");
            sp->fname = fp->fname;
            t += 1;
            puts("before sp->next malloc");
            sp->next = malloc(sizeof(LNode));
            puts("after sp->next");
            sp = sp->next; 
            puts("after sp");
        }
        fp = fp->next;
    }
    sp->next = NULL;
    return t;
}

// Create a list of command line commands
char *cmds[12] = {"ls", "grep", "bash", "vi", "ps", "who", "df", "fortune", "history", "date", "top", "cat"};

// Create corresponding command line argument strings
LNode *shfiles; // Files ending in .sh
LNode *files; // List of all files in directory

// Random words to grep
char *randwords[6] = {"if", "while", "time", "char", "print", "name"};
int nw = 6; // Number of words in array

// Main function
int main(void)
{
    puts("beginning of main");
    // Create vars to hold number of files and .sh files
    int nf = 0;
    int ns = 0;

    files = malloc(sizeof(LNode));
    shfiles = malloc(sizeof(LNode));

    puts("intialize variables");


    printf("%p\n",(void*)files);
    // Populate our lists
    nf = makeFileList(files);
    puts("after makeFileList");
    printf("%p\n",(void*)files);
    ns = getSHFiles(files, nf, shfiles);

    puts("populate directory");

    // Set up a few things for using makeargv() later
    char delim[] = " \t";
    char **destargv;

    puts("after argv stuff");

    // Create a pid_t to store the pid of the child process we are making
    pid_t childpid;
    
    // Make a child process
    childpid = fork();

    // Check that it was successful
    if (childpid == -1)
    {
        perror("Failed to fork");
    }

    // If this is the child:
    if (childpid == 0)
    {
        // Generate a random number to pick a command
        int rndCmd = rand() % 12;

        // Check which command it is to see if an argument is needed
        // 1 is grep, 2 is bash, 3 is vi
        if (rndCmd == 1)
        {
            // Pick a random word to grep for
            int rndW = rand() % nw;
            // Execute bash 
            const char *cmd = cmds[rndCmd];
            const char *argFile = randwords[rndW];
            execlp(cmds[rndCmd], cmd, argFile, NULL);
        }
        else if (rndCmd == 2)
        {
            // Pick a random .sh file to run with bash
            int rndSH = rand() % ns;
            // Execute bash
            const char *cmd = cmds[rndCmd];
            const char *argFile;
            LNode *sp = shfiles;
            for (int i = 0; i < ns; i++)
            {
                if (i == rndSH)
                    argFile = shfiles->fname;
                else
                    sp = sp->next;
            }
            execlp(cmds[rndCmd], cmd, argFile, NULL);
        }
        else if (rndCmd == 3)
        {
            // Pick a random file to open in vi
            int rndF = rand() % nf;
            // Execute vi
            const char *cmd = cmds[rndCmd];
            const char *argFile;
            LNode *fp = files;
            for (int i = 0; i < nf; i++)
            {
                if (i == rndF)
                    argFile = files->fname;
                else
                    fp = fp->next;
            }
            execlp(cmds[rndCmd], cmd, argFile, NULL);
        }
        else
        {
            // Just run the command
            const char *cmd = cmds[rndCmd];
            execlp(cmds[rndCmd], cmd, NULL);
        }
    }
    if (childpid != r_wait(NULL))
    {
        perror("Parent failed to wait");
        return 1;
    }
    return 0;
}
