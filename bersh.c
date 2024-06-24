#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pwd.h>
#include <errno.h>
#include <limits.h>


//constants
#define BERSH_BUF_SIZE 1024 // 1024 bytes
#define BERSH_TOK_BUFSIZE 64// token buffer size
#define BERSH_TOK_DELIM " \t\r\n\a" //token delimiter string


//function declarations
int bersh_cd(char **args);
int bersh_help(char **args);
int bersh_exit(char **args);
int bersh_num_builtins();
int bersh_cd(char **args);
int bersh_help(char **args);
int bersh_exit(char **args);
char *bersh_read_line(void);
char **bersh_split_line(char *line);
int bersh_launch(char **args);
int bersh_execute(char **args);
void bersh_loop(void);
char *get_hostname();
char *get_username();
int bersh_pipe(char **args);
void load_rc_file();

char *builtin_str[] = {
	"|",
	"cd",
	"help",
	"exit",
};

//map keywords to functions
int(*builtin_func[]) (char **) = {
	&bersh_pipe,
	&bersh_cd,
	&bersh_help,
	&bersh_exit,
};

int main(void){

	load_rc_file();
	bersh_loop();
	return 0;
}

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

	//check for pipe 
	for(int i = 0;args[i]!= NULL;i++){
		if(strcmp(args[i],"|") == 0){
			return bersh_pipe(args);
		}
	}

	for(int i=1;i<bersh_num_builtins();i++){
		 if(strcmp(args[0],builtin_str[i]) == 0){
		 	return(*builtin_func[i])(args);
		 }
	}
		 return bersh_launch(args);
}

char *get_hostname(){
	static char hostname[HOST_NAME_MAX];
	if(gethostname(hostname,sizeof(hostname)) == -1){
		perror("gethostname");
		return NULL;
	}
	return hostname;
}

char *get_username(){
	struct passwd *pw;
	errno = 0;
	pw = getpwuid(getuid());
	if(!pw){
		if(errno){
			perror("getpwuid");
		}
		return NULL;
	}
	return pw ->pw_name;
}

void bersh_loop(void){
	char *line;
	char **args;
	int status;
	do{
		char *hostname = get_hostname();
		char *username = get_username();

		if(hostname!=NULL && username != NULL){
			printf("%s@%s:$ ",username,hostname);
		}
		else{
			printf("#$: ");

		}
		line = bersh_read_line();
		args = bersh_split_line(line);
		status = bersh_execute(args);
		
		free(line);
		free(args);
	} while(status);
}

int bersh_pipe(char **args){
	int pipe_position = -1;
for(int i = 0;args[i]!= NULL;i++){
		if(strcmp(args[i],"|") == 0){
			pipe_position = i;
			break;
		}
}
if(pipe_position !=-1){
	char **args1 = malloc(sizeof(char *) * (pipe_position + 1));
	char **args2 = malloc(sizeof(char *) * (BERSH_TOK_BUFSIZE - pipe_position));

		if(!args1 || !args2){
			fprintf(stderr,"bersh: allocation error\n");
			exit(EXIT_FAILURE);
		}

		for(int i=0;i<pipe_position;i++){
			args1[i] = args[i];
		}
		int j = 0;
		for(int i=pipe_position+1;args[i] != NULL;i++){
			args2[j++] = args[i];
		}
		args2[j] = NULL;

		int pipe_fd[2];
		pid_t pid1,pid2;
		int status;

		if(pipe(pipe_fd) == -1){
			perror("pipe");
			exit(EXIT_FAILURE);
		}

		pid1 = fork();
		if(pid1 == 0){
			close(pipe_fd[0]);
			dup2(pipe_fd[1],STDOUT_FILENO);
			close(pipe_fd[1]);
			
			if(execvp(args1[0],args1) == -1){
				perror("bersh");
				exit(EXIT_FAILURE);
			} 
		} else if(pid1<0){
			perror("bersh");
		}
		else{
			pid2 = fork();
			if(pid2 == 0){
			            // Child 2: Execute the second command
            close(pipe_fd[1]);  // Close unused write end of the pipe
            dup2(pipe_fd[0], STDIN_FILENO);  // Redirect stdin to the pipe
            close(pipe_fd[0]);  // Close the pipe read end

            if (execvp(args2[0], args2) == -1) {
                perror("bersh");
                exit(EXIT_FAILURE);
            }
        } else if (pid2 < 0) {
            perror("bersh");
        } else {
            // Parent process
            close(pipe_fd[0]);
            close(pipe_fd[1]);
            waitpid(pid1, &status, 0);
            waitpid(pid2, &status, 0);
        }
    }

		free(args1);
		free(args2);

		} else { fprintf(stderr,"bersh: could not pipe\n");
	exit(EXIT_FAILURE);

	}

	return 1;
	}

	void load_rc_file(){
		FILE *rc_file = fopen(".bershrc","r");
		if(rc_file == NULL){
			perror("Error opening .bershrc");
			return;
		}
		
		char line[BERSH_BUF_SIZE];
		while(fgets(line, sizeof(line), rc_file) != NULL){
			if(line[0] == '\n' || line[0] == '#'){
				continue;
			}
		
		char *command = strtok(line,"\n");
		if(command!=NULL){
			bersh_execute(bersh_split_line(command));
		}
	}
	fclose(rc_file);
}
			
		
	

		
