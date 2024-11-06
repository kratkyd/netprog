#include <stdio.h>
#include <string.h>
#include "client_commands.c"

typedef void (*commandFunc)();

typedef struct {
	char *command;
	commandFunc function;
} Command;

Command commands[] = {
	{"hello", say_hello},
	{"exit", exit_program},
	{"sip", set_server_ip},
	{"connect", server_connect}
};

void execute_command(char *command){
	for (size_t i = 0; i < sizeof(commands)/sizeof(commands[0]); ++i){
		if (strcmp(command, commands[i].command) == 0){
			commands[i].function();
			return;
		}
	}
}

void command_listen(){
	char command[30];
	while(1){
		scanf("%s", command);
		execute_command(command);
	}
}

void setup_user(char *str){
	printf("connected as: %s\n", str);
	strcpy(name, str); 
	printf("%s\n", name);
}

int main(int argc, char *argv[]){
	if (argc != 2) {
		fprintf(stderr, "use: ./client [name]\n");	
		exit(0);
	}
	setup_user(argv[1]);
	command_listen();
	return 0;
}
