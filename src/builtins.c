#include "shell.h"
#include "path.h"
#include "executor.h"
#include "builtins.h"

static int handle_type ( char *command ) {

		if ((strncmp(command, "echo", 4)  == 0) ||
		   ((strncmp(command, "exit", 4)) == 0) ||  
		   ((strncmp(command, "type", 4)) == 0) ||
		   ((strncmp(command, "pwd" , 3)) == 0) ||
		   ((strncmp(command, "cd"  , 2)) == 0)) {
			printf("%s is a shell builtin\n", command);
		} 

		else {
			find_exe_path(command);	
		}
	return 0;
}

int input_handler (char *command) {

	/* Input handler for built in commands */
	if (strncmp(command, "echo", 4) == 0 ) { 
		/* Needs so much improvement ig lol */
		if(*(command+5)== '\'') printf("found \n");
		printf("%ld \n", strlen(command+5));
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
			char absolute_path[MAX_ARGS];
			snprintf(absolute_path, MAX_ARGS, "%s%s", home_path, command+4);
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
	
	/* Input handling for other executables found in $PATH*/
	char *command_cpy = strdup(command);
	handle_path_exe(command_cpy);
	return 0;
}



