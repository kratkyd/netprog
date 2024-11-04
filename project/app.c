#include <stdio.h>
#include <string.h>
#include "app_commands.c"

typedef void (*commandFunc)();

typedef struct {
	char *command;
	commandFunc function;
} Command;

Command commands[] = {
	{"hello", say_hello},
	{"myip", show_my_ip},
	{"exit", exit_program},
	{"server", server}
};

void execute_command(char *command){
	for (size_t i = 0; i < sizeof(commands) / sizeof(commands[0]); ++i){
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

int main(void){

	command_listen();
	return 0;
}
