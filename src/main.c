#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>

/* TODO:
 * there is no single shell like feature will work right now lol
 * Error handling sucks 
 * code is absolute mess
 * Poor memory management
 */

#define MAX_COMMAND_SIZE 1024
#define MAX_HOME_DIR_SIZE  50 /* Really idk */

/*No use for this all because of execvp() did all the job pew!*/

typedef enum {
	TYPE,
	EXEC,
}PathFlag;

/* Function prototypes */
int input_handler(char *command);
int handle_type (char *command);
int find_exe_path(char *command, PathFlag flag);



int main(void) {
  /* Flush after every printf */
  setbuf(stdout, NULL);
  
  while(1) {

	  printf("$ ");

	  /* Define a char array to store the string never try to use string literals char *p these are read only */
	  char command[MAX_COMMAND_SIZE+1];
	  /* scanf is not good for strings use fgets() */
	  fgets(command, MAX_COMMAND_SIZE+1, stdin);
	  /* If the stdin is '\n' do nothing */
	  if (strcmp(command, "\n") == 0 ) {
		  continue;
	  }
	  /* fgets() take the new line as well we need to trim the last character.*/
	  command[strcspn(command, "\n")] = '\0';

	  if (strncmp(command, "exit", 4) == 0 ) {
		break; 
	  }

	  input_handler(command);
	}
  return 0;
}


int input_handler ( char *command ) {

	if (strncmp(command, "echo", 4) == 0 ) { 
		printf("%s \n", command+5);
		return 0;
	}

	if (strncmp(command, "type", 4) == 0) {
		handle_type(command+5);
		return 0;
		}
	
	if (strncmp(command, "pwd", 3) == 0) {
		char pwd_addr[MAX_COMMAND_SIZE]; /*Do we need to allocate this much idk rn the time is 01:58 AM :(*/	
		printf("%s \n", getcwd(pwd_addr, MAX_COMMAND_SIZE)); 
		return 0;
	}

	if (strncmp(command, "cd", 2) == 0){
		int ret;
		if(strncmp(command+3, "~", 1) == 0){

			/*chdir cannnot handle ~*/
			char *home_path = getenv("HOME");
			char absolute_path[MAX_HOME_DIR_SIZE];
			snprintf(absolute_path, MAX_HOME_DIR_SIZE, "%s%s", home_path, command+4);
			ret = chdir(absolute_path);

		}
		else {
			ret = chdir(command+3);
		}

		if(ret == -1) {
			printf("cd: %s: No such file or directory \n", command+3);
		}

		return 0;
	}

	char *command_cpy = strdup(command);
	if (command_cpy){
	
		char *token = strtok(command_cpy, " ");
		char *arr[MAX_COMMAND_SIZE+1]; /* Needs Improvement */
		char *exe_name = token;
		int i = 0;
		for(; token != NULL; i++){		
			arr[i] = token;
			token = strtok(NULL, " ");
		}
		arr[i] = NULL;

		/* Simply running exec() family of functions will terminate current process and 
		 * replace it with the new function i.e what is inside in execvp() 
		 * use fork() to create a copy of parent process that is our shell is the parent 
		 */

		pid_t pid;
		pid = fork();
		if ( pid == -1 ){
			perror("fork");
		}
		if (pid == 0){
			int ret = execvp(exe_name, arr);
			if ( ret == -1 ) {
				//perror ("execvp");
				printf("%s: command not found\n", exe_name);
				exit(1);
			}
		}

		else {
			int status;
			waitpid(pid, &status, 0); /*Need to learn more around these wait, pid, fork, exec*/
		}

		/* Seriously no use for this since this execvp searched for me the exe in $PATH fck!!
		 * PathFlag isExec = find_exe_path(exe_name, EXEC);
		 * if (isExec == EXEC){
		 *	printf("working exec \n");
		 *	}
		 */
	}
		
	return 0;
}

int handle_type ( char *command ) {

		if ((strncmp(command, "echo", 4)  == 0) ||
		   ((strncmp(command, "exit", 4)) == 0) ||  
		   ((strncmp(command, "type", 4)) == 0) ||
		   ((strncmp(command, "pwd" , 3)) == 0) ||
		   ((strncmp(command, "cd"  , 2)) == 0)) {
			printf("%s is a shell builtin\n", command);
		} 

		else {
			find_exe_path(command, TYPE);	
		}
	return 0;
}

int find_exe_path (char *command, PathFlag flag) {

	bool EXE_FOUND = false;
	char *search_tar = command;
	char *path_env = getenv("PATH");
	/* Important to use the copy since the pointer is to the original $PATH
	 * string and strtok will modify that 
	 */

	char *path_env_cpy = strdup(path_env); 		
	if (path_env_cpy){
		char *token = strtok(path_env_cpy, ":");
		while(token){

			/* DIR is a tpye from dirent.h
			 * opendir() is type of DIR
			 * readdir() is type of struct dirent 
			 */

			DIR *dir = opendir(token);

			if (dir){
				struct dirent *file_entry;
				while ((file_entry = readdir(dir)) != NULL ){
					if ( strcmp(file_entry->d_name, search_tar) == 0 ){
						struct stat s_buf;
						char absolute_path[MAX_COMMAND_SIZE];
						snprintf(absolute_path, MAX_COMMAND_SIZE, "%s/%s", token, file_entry->d_name);

						stat(absolute_path, &s_buf);
					
						/* (S_IXUSR | S_IXGRP | S_IXOTH)  = 73 or 1001001 then the file is executable 
						 * ((s_buf.st_mode & S_IFMT) == S_IFREG) checks whether it is a regular file and 
						 * i really dont know wtf is S_IFMT 
						 */

						if (((s_buf.st_mode & S_IFMT) == S_IFREG) &&
							(( s_buf.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) != 0))
						{
							if (flag == TYPE) printf("%s is %s \n", file_entry->d_name, absolute_path);
							if (flag == EXEC) return EXEC;
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

	return 0;
}
