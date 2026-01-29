#include "shell.h"

int handle_path_exe (char *command_cpy) {

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
