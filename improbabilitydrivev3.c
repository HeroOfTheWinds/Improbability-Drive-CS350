//CS 350 - Processes Group - "CTRL+ALT+BELIEVE"
// improbabilitydrive.c - The Improbability Drive
// This program behaves in a very improbable way.

#include <errno.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

// Create a linked list struct
typedef struct LNode LNode;
struct LNode
{
    char fname[256];
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

// Function to get the total number of files in the directory
int getFCount()
{
	// Special thanks to stackoverflow.com for info on how to get file names
    // Create a new directory object (using dirent.h)
    DIR *d;
    // Make a dirent struct
    struct dirent *dir;
    // Try opening the current directory
    d = opendir(".");
    // Create an index variable for incrementing total number of files
    int total = 0;
    
    // If directory opened successfully:
    if (d)
    {
        // While anything to read remains
        while ((dir = readdir(d)) != NULL)
        {
            // Increment file total
            total += 1;
        }
        
		// No need to leave open.
        closedir(d);
    }
    return total;
}

// Function to get the nth filename in a directory
char* getFileName(int n)
{
	// Create a new directory object (using dirent.h)
    DIR *d;
    // Make a dirent struct
    struct dirent *dir;
    // Try opening the current directory
    d = opendir(".");
	
	// Create an indexing variable
	int i = 0;
	
	// If directory opened successfully:
    if (d)
    {
        // While anything to read remains
        while ((dir = readdir(d)) != NULL)
        {
            // Check if this is the nth file
			if (i == n)
			{
				// Return the current file name
				char* fname = malloc(sizeof(char)*strlen(dir->d_name));
				strcpy(fname, dir->d_name);
				return fname;
			}
        }
        
		// No need to leave open.
        closedir(d);
    }
	
	// If code sees this point, error... Return NULL
	return NULL;
}

// Function to verify if given file is an .sh file 
// 1 = true, 0 = false
int isSHFile(char* fname)
{
	// Test if it has the .sh extention
	char *dot = strrchr(fname,'.');
	if (dot && !strcmp(dot, ".sh"))
	    return 1;
	else
	    return 0;
}

// Create a list of command line commands
char *cmds[12] = {"ls", "grep", "bash", "vi", "ps", "who", "df", "fortune", "history", "date", "top", "cat"};

// Random words to grep
char *randwords[6] = {"if", "while", "time", "char", "print", "name"};
int nw = 6; // Number of words in array

// Main function
int main(void)
{
    // Seed random number generator
	time_t t;
	srand((unsigned) time(&t));
	
    // Get number of files
	int nf = 0;
	nf = getFCount();
	
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
			puts("Executing grep");
            // Pick a random word to grep for
            int rndW = rand() % nw;
            // Execute bash 
            const char *cmd = cmds[rndCmd];
            const char *argFile = randwords[rndW];
            execlp(cmds[rndCmd], cmd, argFile, NULL);
        }
        else if (rndCmd == 2)
        {
			puts("Executing bash");
            // Pick a random .sh file to run with bash
            int rndSH = rand() % nf;
			
			char* fname;
			
			// Check if not a SH file, restart search if not
			while ((fname = getFileName(rndSH)))
			{
				if (!isSHFile(fname))
				{
			        rndSH += 1;
				    if (rndSH > nf)
				        rndSH = 0;
				}
				else
				{
					// Execute bash
					const char *cmd = cmds[rndCmd];
					const char *argFile = fname;
					execlp(cmds[rndCmd], cmd, argFile, NULL);
					break;
				}
			}
			
            
        }
        else if (rndCmd == 3)
        {
			puts("Executing vi");
            // Pick a random file to open in vi
            int rndF = rand() % nf;
			
			// Get file name
			char* fname = getFileName(rndF);
			printf("Filename: %s\n", fname);
            // Execute vi
            const char *cmd = cmds[rndCmd];
            const char *argFile = fname;
            execlp(cmds[rndCmd], cmd, argFile, NULL);
        }
        else
        {
            // Just run the command
            const char *cmd = cmds[rndCmd];
			printf("Executing %s\n", cmds[rndCmd]);
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
