#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MAX_COMMAND_SIZE 1024

bool CMD_NOT_FOUND = true;


// Function prototypes
int input_handler(char *command);
//int stat(char *, struct stat *);


int main(int argc, char *argv[]) {
  // Flush after every printf
  setbuf(stdout, NULL);
  
  while(1) {

	  printf("$ ");

	  // Define a char array to store the string never try to use string literals char *p these are read only 
	  char command[MAX_COMMAND_SIZE+1];
	  // scanf is not good for strings use fgets()
	  fgets(command, MAX_COMMAND_SIZE+1, stdin);
	  // If the stdin is '\n' do nothing 
	  if (strcmp(command, "\n") == 0 ) {
		  continue;
	  }
	  // fgets() take the new line as well we need to trim the last character.
	  command[strcspn(command, "\n")] = '\0';

	  if (strncmp(command, "exit", 4) == 0 ) {
		break; 
	  }
	  CMD_NOT_FOUND = true;
	  input_handler(command);
	  if ( CMD_NOT_FOUND )	{
		  printf("%s: command not found \n", command);
	}
	  
	}
  return 0;
}


int input_handler ( char *command ) {

	if (strncmp(command, "echo", 4) == 0 ) {
		CMD_NOT_FOUND = false; 
		printf("%s \n", command+5);
		return 0;
	}

	//CMD_NOT_FOUND need to improve this

	if (strncmp(command, "type", 4) == 0 ) {

		CMD_NOT_FOUND = false;

		if ((strncmp(command+5, "echo", 4)  == 0) ||
		   ((strncmp(command+5, "exit", 4)) == 0) ||  
		   ((strncmp(command+5, "type", 4)) == 0) ||
		   ((strncmp(command+5, "pwd" , 3)) == 0) ||
		   ((strncmp(command+5, "cd"  , 2)) == 0)) {
			printf("%s is a shell builtin\n", command+5);
		} 

		else {
			bool EXE_FOUND = false;
			char *search_tar = command+5;
			//printf("%s \n", command+5);
			char *path_env = getenv("PATH");
			char *path_env_cpy = strdup(path_env); /*Important to use the copy since the pointer is to the original $PATH string and strtok will modify that */
			if (path_env_cpy){
				char *token = strtok(path_env_cpy, ":");
				while(token){
		
					/** DIR is a tpye from dirent.h
				 	  * opendir(), readdir() and closedir() are types of DIR */
					DIR *dir = opendir(token);
	
					if (dir){
						struct dirent *file_entry;
						while ((file_entry = readdir(dir)) != NULL ){
							if ( strcmp(file_entry->d_name, search_tar) == 0 ){
								struct stat s_buf;
								char absolute_path[MAX_COMMAND_SIZE];
								snprintf(absolute_path, MAX_COMMAND_SIZE, "%s/%s", token, file_entry->d_name);

								stat(absolute_path, &s_buf);
								//printf("S_IXUSR: %b, S_IXGRP: %b, S_IXOTH: %b \n", S_IXUSR, S_IXGRP, S_IXOTH);
							
								/** (S_IXUSR | S_IXGRP | S_IXOTH)  = 73 or 1001001 then the file is executable 
								  * ((s_buf.st_mode & S_IFMT) == S_IFREG) checks whether it is a regular file and 
								  * i really dont know wtf is S_IFMT 
								  */
	
								if (((s_buf.st_mode & S_IFMT) == S_IFREG) &&
								   (( s_buf.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) != 0))
								{
									printf("%s is %s \n", file_entry->d_name, absolute_path);
									EXE_FOUND = true;

								}
							}
						}
						
					}
					closedir(dir);
					if (EXE_FOUND){
	
						break;
					}
					token = strtok(NULL, ":");
				}
			}
			free(path_env_cpy);
			if (!EXE_FOUND) {
				 printf("%s: not found\n", search_tar);
			}
		}
	}
		
	return 0;
}
