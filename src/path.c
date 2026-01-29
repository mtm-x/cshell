#include "shell.h"

int find_exe_path (char *command) {

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

	return 0;
}
