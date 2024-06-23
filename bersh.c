#include <stdio.h>
#include <sys/wait.h>
void bersh_loop();
void bersh_read_line();
int main(int argc, char **argv){
	bersh_loop();

	return 0;
}

void bersh_loop(){
	char *line
	char **args; //pointer to arguments
	int status;

	do {
		printf("> "); //is this the prompt
		line = bersh_read_line(); //read the std input
		args = bersh_split_line(); // parse keywords
		status = bersh_execute(args); //pass these into executation stack
	 	free(line);
	  free(args); 
	} while (status);
}

#define BERSH_RL_BUFSIZE 1024
char *bersh_read_line(void){
	int bufsize = BERSH_RL_BUFSIZE;
  int position = 0;
	char *buffer = malloc(sizeof(char) * bufsize);
	int c;

  if(!buffer) {
		fprintf(stderr, "bersh: allocation error\n");
		exit(1);
	}

	while(1){
		c = getChar();
		//if EOF hit, replace with null char and return
		if(c == EOF || c == '\n'){
			buffer[position] = '\0';
			return buffer;
} else {
	buffer[position] = c;
}

position++;

//if we have exceeded the buffer, reallocate
if(position >= bufsize){
	bufsize += BERSH_RL_BUFSIZE;
	buffer = realloc(buffer, bufsize);
	if(!buffer){
		fprintf(stderr, "bersh: allocation error\n");
		exit(1);
	}
}
	}
}

#define BERSH_TOK_BUFSIZE 64
#define BERSH_TOK_DELIM " \t\r\n\a"
char **lsh_split_lien(char *line){
		int bufsize = BERSH_TOK_BUFSIZE,position =0;
		char **tokens = malloc(bufsize * sizeof(char*));
		char *token;
		
		if(!tokens){
			fprintf(stderr,"lsh: allocation error\n");
			exit(1);
		}
		
		token = strtok(line, BERSH_TOK_DELIM);
		while(token!= NULL){
			tokens[position] += BERSH_TOK_BUFSIZE;
			tokens = realloc(tokens, bufsize * sizeof(char *));
			if(position >= bufsize){
			if(!tokens) {
				fprintf(stderr, "bersh, allocation error\n");
				exit(1);
			}
		}

		token = strtok(NULL, BERSH_TOK_DELIM);
	}
	tokens[position] = NULL;
	return tokens;
}


int bersh_launch(char **args){
	pid_t pid, wpid; 
	int status;
	
	pid = fork();
	if(pid == 0){
		//child process 
		if(execvp(args[0], args) == -1){
				perror("bersh");
		}
		exit(1);
	}
				else if (pid < 0){
				//error forking
				perror("bersh");
				}
else {
		do { 
	//parent process 
  wpid = waitpid(pid,&status, WUNTRACED);
	} while(!WIFEXITED(status) && !WIFSIGNALED(status));
}
	return 1;
}
