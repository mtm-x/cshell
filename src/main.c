#include "shell.h"
#include "builtins.h"

/* TODO:
 * there is no single shell like feature will work right now lol
 * Error handling sucks 
 * code is absolute mess
 * Poor memory management
 */

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





