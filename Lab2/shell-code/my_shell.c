#include  <stdio.h>
#include  <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include<signal.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64
#define MAX_BACKGROUND_PROCESSES 64
#define MAX_FOREGROUND_PROCESSES 64
pid_t background_processes[MAX_BACKGROUND_PROCESSES], foreground_processes[MAX_FOREGROUND_PROCESSES];
int background = 0,serial = 0, parallel =0, foreground_pid = 0, num_background = 0, num_foreground = 0, flag_kill=0;

void signal_handler(int sig){
	if(!background && foreground_pid){
		if(parallel){
			for(int i=0; i<num_foreground;i++){
				if(kill(foreground_processes[i], 0)==0)
				kill(foreground_processes[i], SIGKILL);
			}
		}
		else{
		kill(-(foreground_pid), SIGKILL);		
		flag_kill = 1;
		}
	}
	return;
}
/* Splits the string by space and returns the array of tokens
*
*/
char **tokenize(char *line)
{
  char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
  char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
  int i, tokenIndex = 0, tokenNo = 0;

  for(i =0; i < strlen(line); i++){

    char readChar = line[i];

    if (readChar == ' ' || readChar == '\n' || readChar == '\t'){
      token[tokenIndex] = '\0';
      if (tokenIndex != 0){
	tokens[tokenNo] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
	strcpy(tokens[tokenNo++], token);
	tokenIndex = 0; 
      }
    } else {
      token[tokenIndex++] = readChar;
    }
  }
 
  free(token);
  tokens[tokenNo] = NULL ;
  return tokens;
}


int main(int argc, char* argv[]) {
	char  line[MAX_INPUT_SIZE];            
	char  **tokens;              
	int i, l =0;


	signal(SIGINT, signal_handler);
	while(1) {	
		l=0;
		serial =0;
		parallel = 0;
		flag_kill = 0;
		num_foreground =0;
		background = 0;


		int terminated_background = -1;	
		for(int i=0;i<num_background;i++){
			terminated_background = waitpid(background_processes[i], NULL, WNOHANG);
			if(terminated_background > 0){
				printf("Shell: Background process finished");
				break;
			}
		}
		if(terminated_background>0){
			int flag = 0;
			for(int i=0; i<num_background; i++){
				if(background_processes[i]==terminated_background){flag = i; break;}
			}
			for(int i=flag; i<num_background-1;i++){
				background_processes[i]=background_processes[i+1];
			}
			num_background--;
		}


		bzero(line, sizeof(line));
		printf("$ ");
		scanf("%[^\n]", line);
		getchar();


		line[strlen(line)] = '\n'; //terminate with new line
		tokens = tokenize(line);
		while(tokens[l]){
			if(strcmp(tokens[l], "&&")==0) serial = 1;
			if(strcmp(tokens[l], "&&&")==0 ) {parallel =1;}
			if(strcmp(tokens[l], "&") == 0) {  tokens[l] = NULL; background = 1; num_background++;}
			l++;
		}

       	if(!tokens[0]) continue;

		if(!strcmp(tokens[0], "exit")){
			// kill(-getpid(), SIGTERM);
			for(int i=0;i<num_background;i++){
				if(kill(background_processes[num_background],0)==0)
				{
					kill(background_processes[i], SIGTERM);
					waitpid(background_processes[i], NULL, 0);
				}
			}
			for(int i=0;tokens[i]!=NULL;i++){
				free(tokens[i]);
			}
			free(tokens);
			break;
		}
	   	if(strcmp(tokens[0],"cd") ==0){
			int ret = chdir(tokens[1]);
			if(ret == -1){
				printf("Shell: Incorrect command\n");
			}

	   	}
	   	else{

			if(background){
				printf("%s  %s", tokens[0], tokens[1]);
				int stat;
				pid_t pid = fork();
				if(pid<0){
					printf("Error in forking\n");
				}
				else if(pid == 0){
					setpgid(0,getppid());
					execvp(tokens[0], tokens);
					printf("Error in executing the command\n");
					exit(100);
				}
			}
			else{
				if(serial){
					char **serial_tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
					int i=0,j=0, f=0;
					pid_t pgid;
					while(!flag_kill && tokens[i]!=NULL){
						if(!strcmp(tokens[i], "&&")){
							serial_tokens[j]=NULL;
							int stat;
							pid_t pid = fork();
							if(pid<0){
								printf("Error in forking\n");
							}
							else if(pid == 0){
								if(f==0){pgid = getpid(); f=1;}
								setpgid(0,pgid);
								execvp(serial_tokens[0], serial_tokens);
								printf("Error in executing the command\n");
								exit(100);
							}
							else{
								if(f==0) foreground_pid = pid;
								waitpid(pid, &stat, 0);
								if(WEXITSTATUS(stat)==100) printf("Couldn't execute the command\n");
							}
							j=0;
							for(int k=0;k<MAX_NUM_TOKENS;k++){ if(serial_tokens[k]) free(serial_tokens[k]);} 
						}
						else{
							serial_tokens[j] = (char *)malloc(MAX_TOKEN_SIZE* sizeof(char)); strcpy(serial_tokens[j],tokens[i]); j++;
						}
						i++;
					}
					if(!flag_kill){
						serial_tokens[j]=NULL;
						int stat;
						pid_t pid = fork();
						if(pid<0){
							printf("Error in forking\n");
						}
						else if(pid == 0){
							if(f==0){pgid = getpid(); f=1;}
							setpgid(0,pgid);
							execvp(serial_tokens[0], serial_tokens);
							printf("Error in executing the command\n");
							exit(100);
						}
						else{
							if(f==0)foreground_pid = pid;
							waitpid(pid, &stat, 0);
							if(WEXITSTATUS(stat)==100) printf("Couldn't execute the command\n");
						}
					}
					// for(int k=0;k<MAX_NUM_TOKENS;k++){ if(serial_tokens[k]) free(serial_tokens[k]);} 

				}
				else if(parallel){
					char** parallel_tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
					int i=0,j=0,f=0;
					pid_t pgid;
					while(tokens[i]!=NULL){
						if(!strcmp(tokens[i], "&&&")){
							parallel_tokens[j]=NULL;
							int stat;
							pid_t pid = fork();
							if(pid<0){
								printf("Error in forking\n");
							}
							else if(pid == 0){
								if(f==0){pgid = getpid(); f=1;}
								setpgid(0,pgid);
								execvp(parallel_tokens[0], parallel_tokens);
								printf("Error in executing the command\n");
								exit(100);
							}
							else{
								if(f==0) foreground_pid = pid;
								foreground_processes[num_foreground++] = pid;
							}
							j=0;
							for(int k=0;k<MAX_NUM_TOKENS;k++){ if(parallel_tokens[k]) free(parallel_tokens[k]);} 
						}
						else{
							parallel_tokens[j] = (char *)malloc(MAX_TOKEN_SIZE* sizeof(char)); strcpy(parallel_tokens[j],tokens[i]); j++;
						}
						i++;
					}
					parallel_tokens[j]=NULL;
					int stat;
					pid_t pid = fork();
					if(pid<0){
						printf("Error in forking\n");
					}
					else if(pid == 0){
						if(f==0){pgid = getpid(); f=1;}
						setpgid(0,pgid);
						execvp(parallel_tokens[0], parallel_tokens);
						printf("Error in executing the command\n");
						exit(100);
					}
					else{
						if(f==0) foreground_pid = pid;
						foreground_processes[num_foreground++] = pid;
					}

					for(int i=0;i<num_foreground;i++){
						waitpid(foreground_processes[i], NULL, 0);
					}

				}
				else{
					int stat;
					pid_t pid = fork();
					if(pid<0){
						printf("Error in forking\n");
					}
					else if(pid == 0){
						setpgid(0,getpid());
						execvp(tokens[0], tokens);
						printf("Error in executing the command\n");
						exit(100);
					}
					else{
						foreground_pid = pid;
						num_foreground=1;
						waitpid(pid, &stat, 0);
						if(WEXITSTATUS(stat)==100) printf("Couldn't execute the command\n");
					}
				}
			}
	   	}
		// Freeing the allocated memory	
		for(i=0;tokens[i]!=NULL;i++){
			free(tokens[i]);
		}
		free(tokens);

	}
	return 0;
}
