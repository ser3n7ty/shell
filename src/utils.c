#include "utils.h"

char *builtin_str[] = {
	"cd",
	"help",
	"exit"
};

int (*builtin_func[]) (char **) = {
	&ysh_cd,
	&ysh_help,
	&ysh_exit
};

void ysh_loop(void) {
	char *line;
	char **args;
	int status;

	do {
		printf(">>>");
		line = ysh_read_line();
		args = ysh_split_line(line);
		status = ysh_execute(args);

		free(line);
		free(args);
	} while (status);
}

char *ysh_read_line(void) {
	int buf_size = YSH_RL_BUFSIZE;
	int position = 0;
	char *buffer = malloc(buf_size * sizeof(char));
	int c;

	if(!buffer) {
		fprintf(stderr,"ysh: Allocate memory error.\n");
		exit(EXIT_FAILURE);
	}
		

	while(1) {
		c = getchar();

		if(c == EOF || c == '\n') {
			buffer[position] = '\0';
			return buffer;
		} else {
			buffer[position++] = c;
		}

		// Address the buffer exceeed
		if(position >= buf_size) {
			buf_size += YSH_RL_BUFSIZE;
			buffer = realloc(buffer, buf_size);
			if(!buffer) {
				fprintf(stderr, "ysh: Allocate memory error.\n");
				exit(EXIT_FAILURE);
			}
		}
	}
	return buffer;
}

//char *ysh_read_line(void) {
//	char *line = NULL;
//	ssize_t buf_size = 0;
//
//	if(getline(&line, &buf_size, stdin) == -1) {
//		if(feof(stdin)) {
//			exit(EXIT_SUCCESS);
//		}
//	} else {
//		perror("readline");
//		exit(EXIT_FAILURE);
//	}
//}

char **ysh_split_line(char *line) {
	int position = 0;
	int arg_size = YSH_ARG_NUMBER;
	char **args = malloc(arg_size * sizeof(char *));
	char *flag;
	char *delim = " ";

	if(!args) {
		fprintf(stderr, "ysh: allocation error.\n");
		exit(EXIT_FAILURE);
	}

	flag = strtok(line, delim);

	while(flag) {
		args[position++] = flag;

		if(position >= arg_size) {
			arg_size += YSH_ARG_NUMBER;
			args = realloc(args, arg_size);
			if(!args) {
				fprintf(stderr, "ysh: allocation error.\n");
				exit(EXIT_FAILURE);
			}
		}

		flag = strtok(NULL, delim);
	}

	args[position] = '\0';
	return args;
}

int ysh_execute(char **args) {
	int i = 0;
	if(args[0] == NULL) {
		return 1;
	}

	// Determine whether it is a built-in command and execute it
	for(int i=0; i < ysh_num_builtins(); i++) {
		if(strcmp(args[0], builtin_str[i]) == 0) {
			return (*builtin_func[i])(args);
		}
	}
	return ysh_launch(args);
}

int ysh_launch(char **args) {
	pid_t pid, wpid;
	int status;

	// fork() 
	// parent and child process will execute the rest code
	// parent process needs to wait
	pid = fork();
	if(pid == 0) {
		if(execvp(args[0], args) == -1) {
			perror("ysh");
		}
		exit(EXIT_FAILURE);
	} else if(pid < 0) {
		perror("lsh");
	} else {
		do {
			wpid = waitpid(pid, &status, WUNTRACED);
		} while(!WIFEXITED(status) && !WIFSIGNALED(status));
	}
	return 1;
}

int ysh_num_builtins() {
	return sizeof(builtin_str) / sizeof(char *);
}

int ysh_cd(char **args) {
	if(args[1] == NULL) {
		fprintf(stderr, "ysh: expected argument \" cd \"\n");
	} else {
		if(chdir(args[1]) != 0) {
			perror("ysh");
		}
	}
	return 1;
}

int ysh_help(char **args) {
	printf("Ser3n7ty's ysh");
	printf("Type program names and arguments, and hit enter.\n");
	printf("The following are built in:\n");
	for(int i=0; i < ysh_num_builtins(); ++i) {
		printf("\t%s\n", builtin_str[i]);
	}
	printf("Use the man command for information on other programs.\n");
	return 1;
}

int ysh_exit(char **args) {
	return 0;
}


