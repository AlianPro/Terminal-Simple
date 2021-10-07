#include<stdlib.h>
#include<stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int func_cd(char **args);
int func_help(char **args);
int func_exit(char **args);

int executarComando(char** args);

char *builder_str[] = {
  "cd",
  "help",
  "exit"
};

int (*builder_func[]) (char **) = {
	&func_cd,
	&func_help,
	&func_exit
};

int num_inclusas() {
	return sizeof(builder_str) / sizeof(char *);
}

int func_cd(char **args){
	if(args[1] == NULL) printf("Esperava argumento para o cd. \n");
	else{
		if(chdir(args[1]) != 0) perror("erro");
	}
	return 1;
}

int func_exit(char **args){
	return 0;
}

int func_help(char **args){
	printf("Funcionalidades construidas:\n");

	for(int i=0; i<num_inclusas(); i++){
		printf("->%s\n", builder_str[i]);
	}
	return 1;
}

int func_execute(char **args){
	int i;
	if(args[0]==NULL) 
		return 1;
	for(i=0; i<num_inclusas(); i++){
		if(strcmp(args[0], builder_str[i]) == 0) 
			return (*builder_func[i])(args);
	}
	return executarComando(args);
}

int executarComando(char** args){
	pid_t pid, wpid;
	int status;

	pid = fork();

	if(pid==0){
		if(execvp(args[0], args) == -1) perror("erro");
		exit(EXIT_FAILURE);
	}else if(pid < 0){
		perror("erro");
	}else{
		do{
			wpid = waitpid(pid, &status, WUNTRACED);
		}while(!WIFEXITED(status) && !WIFSIGNALED(status));
	}
	return 1;
}

#define TOKEN_BUFSIZE 64
#define delimitador " \t\r\n\a"
char **dividirLinha(char *linha){
	int bufsize = TOKEN_BUFSIZE, posicao = 0;
	char **tokens = malloc(TOKEN_BUFSIZE * sizeof(char*));
	char* token;

	if(!tokens){
		printf("Erro de alocacao!\n");
		exit(EXIT_FAILURE);
	}

	token = strtok(linha, delimitador);

	while(token != NULL){
		tokens[posicao] = token;
		posicao++;

		if(posicao >= TOKEN_BUFSIZE){
			bufsize += TOKEN_BUFSIZE;
			tokens = realloc(tokens, bufsize*sizeof(char*));
			if(!tokens){
				printf("Erro de alocacao. \n");
				exit(EXIT_FAILURE);
			}
		}
		token = strtok(NULL, delimitador);
	}
	tokens[posicao] = NULL;
	return tokens;
}

#define LINHA_BUFSIZE 1024
char *lerLinha(void){
	int bufsize = LINHA_BUFSIZE;
	int posicao = 0;
	char *buffer = malloc(sizeof(char) * bufsize);
	int aux;

	if(!buffer){
		printf("Erro de alocação.");
		exit(EXIT_FAILURE);
	}

	while(1){
		aux = getchar();
		if(aux == '\n' || aux == EOF){
			buffer[posicao] = '\0';
			return buffer;
		} else buffer[posicao] = aux;
		posicao++;

		if(posicao >= bufsize){
			bufsize += 1024;
			buffer = realloc(buffer, bufsize);
			if(!buffer){
				printf("Erro de alocacao!\n");
				exit(EXIT_FAILURE);
			}
		}
	}
}

void func_loop(void){
	char* linha;
	char **args;
	int status;

	do{
		printf("-> ");
		linha = lerLinha();
		args = dividirLinha(linha);
		status = func_execute(args);

		free(linha);
		free(args);

	}while(status);
}

int main(int argc, char **argv){
	func_loop();
	return 0;
}