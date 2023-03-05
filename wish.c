#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

int prompt();

int prompt() {
  //prompts the user for a command. Parses it, and then executes it
  char* buffer;
  char exit_code[] = "exit";
  size_t lim = 256;
  size_t characters;
  printf("wish> ");
  char path[] ="/usr/bin /bin";
  
  characters = getline(&buffer,&lim,stdin);

  
  char* delim = " \n";
  char* in_argv[10];
  int i = 1;
  in_argv[0] = strtok(buffer,delim);
  while ((in_argv[i] = strtok(NULL,delim))!=NULL){
    i++;
    if (i == 10) {
      return -3;
    }
  }

  if (strcmp(in_argv[0],exit_code)==0){
    return -2;
  }
    
  //find the path that contains our command
  char exec_path[100];
  char* search_path;
  int ac;
  search_path = strtok(path," ");
  do {
    
    strcpy(exec_path,search_path);
    strcat(exec_path,"/");
    strcat(exec_path,in_argv[0]);
    ac = access(exec_path,X_OK);
    if (ac == 0) break; 
  } while ((search_path = strtok(NULL," ")) != NULL);
    
  if (ac != 0) return -1; // couldn't find it

  in_argv[0] = exec_path;
  
  int rc = fork();
  if (rc < 0) exit(-1);
  else if (rc == 0) {
    execv(in_argv[0],in_argv);
  } else if (rc > 0) {
    wait(NULL);
    return 0;
  }

}

int main() {

  //prompt has several return codes specified here:
  //0 - user entered a command that executed successfully
  //-1 - user entered a command that could not be found
  //-2 - user made exit command. Abort program
  int returncode;
  while ((returncode = prompt())!=-2) {;}
  
  return 0;
}
