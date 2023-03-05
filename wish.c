#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

int prompt();
int iswhitespace(char c);


int iswhitespace(char c){
  return (c == 32) || (c == 9) || (c == 10) || (c==0);
}


int prompt() {
  //prompts the user for a command. Parses it, and then executes it

  char exit_code[] = "exit";
  char cd_code[] = "cd";
  char path_code[] = "path";
  size_t lim = 1000;
  char* buffer;
  size_t characters;
  printf("wish> ");
  char path[1000] ="/usr/bin /bin";
  
  characters = getline(&buffer,&lim,stdin);
  //parse user input. Delimiters inlclude all whitespace characters
  //because arguments must be passed to an array, wish will generate an error
  //if the user gives too many inputs. TODO: unbound in_argc
  char* delim = " \n\t";


  int in_argc = 0;
  
  int whitespace = 1;
  int len = strlen(buffer);
  for (int i = 0; i<len; i++){
    if (whitespace && !iswhitespace(buffer[i])) {
      in_argc++;
    }
    whitespace = iswhitespace(buffer[i]);
  }
  
  char* in_argv[in_argc];
  
  //assign first argument to in_argv. If null, there is no command. Make next prompt
  if ((in_argv[0] = strtok(buffer,delim))==NULL) return 0;
  
  while ((in_argv[in_argc] = strtok(NULL,delim))!=NULL){
    in_argc++;
    if (in_argc == 10) {
      return -1;
    }
  }
  
  //built in command: exit. Throws an error if the user tries to pass arguments
  //i think strtok returns NULL if you've reached the end of the string, so NULL would
  //deliminate the end of the arguments
  if (strcmp(in_argv[0],exit_code)==0){
    if (in_argc!=1) return -1;
    return -2;
  }
  //built in command: cd
  if (strcmp(in_argv[0],cd_code)==0){
    if (in_argc!=2) return -1;
    chdir(in_argv[1]);
    return 0;
  }
  //built-in command: path
  if (strcmp(in_argv[0],path_code)==0){
    path[0] = '\0'; //set path to empty string
    for (int i = 1; i<in_argc; i++){
      strcat(path,in_argv[i]);
    }
    return 0;
  }
    
  //find the path that contains our command
  char exec_path[100];
  char* search_path;
  int ac;
  char* path_cpy;
  strcpy(path_cpy,path); //we don't want to add null characters into the path
  //so we copy it in order to perform parsing
  search_path = strtok(path_cpy," ");
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
  //-1 - general error
  //-2 - user made exit command. Abort program
  int returncode;
  char error_message[30] = "An error has occured\n";
  int errmsg_len = strlen(error_message);
  while ((returncode = prompt())!=-2) {
    if (returncode == -1){
      write(STDERR_FILENO, error_message, errmsg_len); 
    }
  }
  
  return 0;
}
