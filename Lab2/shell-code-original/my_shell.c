#include  <stdio.h>
#include  <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include<signal.h>
#include<sys/wait.h>
#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64
#define MAX_BACKGROUND_PROCESSES 64
pid_t background_processes[MAX_BACKGROUND_PROCESSES];
pid_t foreground_process, foreground_processes[64];
int foreground = 0, num_foreground_processes=0;

void signal_handler(int sig){
	if(foreground && foreground_process){
		kill(-__getpgid(foreground_process), SIGKILL);
	}
}
void signal_handler_parallel(int sig){
	if(foreground && foreground_process)
	killpg(-__getpgid(foreground_process) , SIGKILL);
}

/* Splits the string by space and returns the array of tokens
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
	int l,num_background_processes = 0;
	signal(SIGINT, signal_handler);
	while(1) {
		int background=0,flag=0, parallel=0, series=0; l=0; foreground=0, foreground_process =0; num_foreground_processes=0;	
		
		
		// Checking for termination of a background process every time
		pid_t bg_process = waitpid(-1, NULL, WNOHANG);// waiting every time when taking new input
		if (bg_process > 0) {
			printf("Shell: Background process finished\n");
		}
		for(int i=0;i<MAX_BACKGROUND_PROCESSES;i++){
			if(background_processes[i]==bg_process) { flag = i+1; break;}
		}
		if(flag){
			for(int i=flag;i<MAX_BACKGROUND_PROCESSES;i++){
				background_processes[i-1]=background_processes[i];
			}
			num_background_processes--;
		}


		/* BEGIN: TAKING INPUT */
		bzero(line, sizeof(line));
		printf("$ ");
		scanf("%[^\n]", line);
		/* END: TAKING INPUT */
		getchar();
		line[strlen(line)] = '\n'; //terminate with new line
		tokens = tokenize(line);


		while(tokens[l]!=NULL){
			if(!strcmp(tokens[l],"&&")) series=1;
			else if(!strcmp(tokens[l],"&&&")) parallel=1;
			l++;
		}
		if(!tokens[0]) continue;
		if(!strcmp(tokens[l-1],"&")) 
		{
			background=1;
			tokens[l-1]=NULL;
		}

		if(!strcmp(tokens[0],"exit")){
			for (int i = 0; i < num_background_processes; ++i) {
        		if(kill(background_processes[i], 0) == 0)
				kill(background_processes[i], SIGTERM);
    		}
			for(int i=0;tokens[i]!=NULL;i++){
				free(tokens[i]);
			}
			free(tokens);
			break;
		}


		if(!strcmp(tokens[0],"cd")){
				if (!strcmp(tokens[1], "~")) {
					chdir(getenv("HOME"));
				}
				else if (chdir(tokens[1]) != 0) {
					printf("Shell:Incorrect command\n");
				}
		}
		else{
			// For foreground processes
			if(!background)
			{

				// For foreground processes with serial running
				if(series){
					int i=0,j=0,f=0;
					pid_t my_gpid;
					char **s_tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
					while(tokens[i]){
						if(!strcmp(tokens[i], "&&")){
							s_tokens[j]=NULL;
							pid_t pid= fork();
							if(pid<0){printf("Error in forking\n");}
							else if(pid==0){
								if(!f) {f=1;my_gpid = getpid();}
								setpgid(0,my_gpid);
								int ret=execvp(s_tokens[0],s_tokens);
								if(ret==-1)printf("\tError in executing the command %s\n \t Please enter a valid command\n", tokens[0]);
								exit(1);
							}
							else{foreground_process= pid;foreground = 1;waitpid(pid,NULL,0);}
							j=0;
							for(int k=0;k<MAX_NUM_TOKENS;k++){ if(s_tokens[k]) free(s_tokens[k]);} 
						}
						else{s_tokens[j] = (char *)malloc(MAX_TOKEN_SIZE* sizeof(char)); strcpy(s_tokens[j],tokens[i]); j++;}
						i++;
					}
					s_tokens[j]=NULL;
					pid_t pid= fork();
					if(pid<0){printf("Error in forking\n");}
					else if(pid==0){
						setpgid(0,my_gpid);
						int ret=execvp(s_tokens[0],s_tokens);
						if(ret==-1) printf("\tError in executing the command %s\n \t Please enter a valid command\n", tokens[0]);
						exit(1);
					}
					else{foreground_process= pid; foreground = 1; waitpid(pid,NULL,0);}
					for(int k=0;s_tokens[k]!=NULL;k++){
							if(s_tokens[k])
							free(s_tokens[k]);
					}
					free(s_tokens);
				}	

				//for foreground processes with parallel running
				else if(parallel){
					int i=0,j=0,f=0;
					pid_t my_gpid, foreground_processes[64];
					char **s_tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
					while(tokens[i]){
						if(!strcmp(tokens[i], "&&&")){
							s_tokens[j]=NULL;
							pid_t pid= fork();
							signal(SIGINT,signal_handler_parallel);
							if(pid<0){printf("Error in forking\n");}
							else if(pid==0){
								if(!f) {my_gpid = getpid(); f=1;}
								setpgid(0,my_gpid);
								int ret=execvp(s_tokens[0],s_tokens);
								if(ret==-1)printf("\tError in executing the command %s\n \t Please enter a valid command\n", s_tokens[0]);
								exit(1);
							}
							else{foreground_process= pid;foreground = 1;foreground_processes[num_foreground_processes++]=pid;}
							j=0;
							for(int k=0;k<MAX_NUM_TOKENS;k++) { if(s_tokens[k]) free(s_tokens[k]);}
						}
						else{ s_tokens[j]= (char *)malloc(MAX_TOKEN_SIZE* sizeof(char)); strcpy(s_tokens[j], tokens[i]); j++;}
						i++;
					}
					s_tokens[j]=NULL;
					pid_t pid= fork();
					if(pid<0){printf("Error in forking\n");}
					else if(pid==0){
						setpgid(0,my_gpid);
						int ret=execvp(s_tokens[0],s_tokens);
						if(ret==-1)printf("\tError in executing the command %s\n \t Please enter a valid command\n", tokens[0]);
						exit(1);
					}
					else{foreground_process= pid;foreground = 1;foreground_processes[num_foreground_processes++]=pid;}

					for(int k=0; k<num_foreground_processes;k++){
						waitpid(foreground_processes[k],NULL,0);
					}
					for(int k=0;s_tokens[k]!=NULL;k++){
						if(s_tokens[k])
						free(s_tokens[k]);
					}
					free(s_tokens);
				}

				// For normal foreground processes without sequential or parallel running
				else{
					pid_t pid= fork();
					if(pid<0){printf("Error in forking\n");}
					else if(pid==0){
						setpgid(0,0);
						int ret=execvp(tokens[0],tokens);
						if(ret==-1)printf("\tError in executing the command %s\n \t Please enter a valid command\n", tokens[0]);
						exit(1);
					}
					else { foreground=1 ; foreground_process= pid; waitpid(pid, NULL, 0);}
				}
				
			}

			// For background processes
			else{
				pid_t pid= fork();
				//Error 
				if(pid<0)printf("Error in forking\n");
				else if(pid==0){
					setpgid(0,0);
					int ret=execvp(tokens[0],tokens);
					if(ret==-1)printf("\tError in executing the command %s\n \t Please enter a valid command\n", tokens[0]);
					exit(1);
				}
				background_processes[num_background_processes++] = pid;
			}
	   }
		// Freeing the allocated memory	
		for(int i=0;tokens[i]!=NULL;i++){
			free(tokens[i]);
			tokens[i]=NULL;
		}
		free(tokens);

	}
	return 0;
}
