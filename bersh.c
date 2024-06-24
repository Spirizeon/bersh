#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BERSH_BUF_SIZE 1024 // 1024 bytes
#define BERSH_TOK_BUFSIZE 64// token buffer size
#define BERSH_TOK_DELIM " \t\r\n\a" //token delimiter string

int bersh_cd(char **args);
int bersh_help(char **args);
int bersh_exit(char **args);

char *builtin_str[] = {
	"cd",
	"help",
	"exit",
};

int(*builtin_func[]) (char **) = {
	&bersh_cd,
	&bersh_help,
	&bersh_exit,
};

int bersh_num_builtins(){
	return sizeof(builtin_str)/sizeof(char *);
}

int bersh_cd(char **args){
	if(args[1] == NULL){
		fprintf(stderr,"bersh: expected argument to \"cd\"\n");
	}
	else{
		if(chdir(args[1])!= 0){
			perror("bersh");
		}
	}
	return 1;
}

int bersh_help(char **args){
	printf("spirizeon's BERSH\n");
	printf("=========BERZi-SHell===========\n");
	for(int i=0;i< bersh_num_builtins();i++){
		printf(" %s\n",builtin_str[i]);
	}
	printf("use the man command for more help on each function\n");
	return 1;
}

int bersh_exit(char **args){
	return 0;
}

char *bersh_read_line(void){
	int bufsize = BERSH_BUF_SIZE;
	int position = 0;
	char *buffer = malloc(sizeof(char) * bufsize);
	int c; // char index
	
	//in case of buffer alloc failure
	if(!buffer){
		printf("allocation failed\n");
	}
	
	while(1){
		c = getchar();
		if(c==EOF || c == '\n'){
			buffer[position] = '\0';
			break;
		}
		else{
			buffer[position] = c;
		}
		position++;
	} 

	//on buffer overflow
	if(position >=bufsize){
	bufsize += BERSH_BUF_SIZE;
	buffer = realloc(buffer,bufsize);
	if(!buffer){
		fprintf(stderr,"bersh: allocation error\n");
		exit(EXIT_FAILURE);
		}
	}

	return buffer;
}

char **bersh_split_line(char *line){
	int bufsize = BERSH_TOK_BUFSIZE;
	int position = 0;
	char **tokens = malloc(bufsize * sizeof(char*));
	char *token;

	if(!tokens){
		fprintf(stderr,"bersh: alloaction error\n");
		exit(EXIT_FAILURE);
	}
	
	token = strtok(line,BERSH_TOK_DELIM); //split first delimation
	while(token!=NULL){
		tokens[position] = token;
		position++;
		if(position>=bufsize){ //buffer overflow condition 
 			bufsize += BERSH_TOK_BUFSIZE;
			tokens = realloc(tokens,sizeof(char*));
			if(!tokens){
				fprintf(stderr,"bersh: allocation failure\n");
				exit(EXIT_FAILURE);
			}
		}
		token = strtok(NULL,BERSH_TOK_DELIM);
	}
	tokens[position] = NULL; //terminate string 
	return tokens; //return parsed buffer 
}
	
int bersh_launch(char **args){
	pid_t pid,wpid;
	int status;
	pid = fork();
	if(pid == 0){
		if(execvp(args[0],args) == -1){
			perror("bersh");
			exit(EXIT_FAILURE);
		}
	}
	else if(pid<0){
		perror("bersh");	
	}
	else{
		do{
			wpid = waitpid(pid,&status,WUNTRACED);
		} while(!WIFEXITED(status) && !WIFSIGNALED(status));
	}
	return 1;
}

	

int bersh_execute(char **args){
	int i;
	if(args[0] == NULL){
		//empty prompt
		return 1;
	}

	for(int i=0;i<bersh_num_builtins();i++){
		 if(strcmp(args[0],builtin_str[i]) == 0){
		 	return(*builtin_func[i])(args);
		 }
	}
		 return bersh_launch(args);
}

void bersh_loop(void){
	char *line;
	char **args;
	int status;
	do{
		printf("#$: ");
		line = bersh_read_line();
		args = bersh_split_line(line);
		status = bersh_execute(args);
		
		free(line);
		free(args);
	} while(status);
}


int main(void){
	bersh_loop();
	return 0;
}
