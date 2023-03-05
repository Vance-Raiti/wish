#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int prompt();

int prompt() {
  //prompts the user for a command. Parses it, and then executes it
  char* buffer;
  size_t lim = 256;
  size_t characters;
  printf("wish> ");
  char* path[] = {"/usr/bin","/bin"};
  
  characters = getline(&buffer,&lim,stdin);

  char* delim = " \n";
  char* in_argv[10];
  int i = 1;
  in_argv[0] = strtok(buffer,delim);
  while ((in_argv[i] = strtok(NULL,delim))!=NULL){
    i++;
    if (i == 10) {
      return -2; //too many args
    }
  }
  char exec_path[100];
  strcpy(exec_path,path[0]);
  strcat(exec_path,"/");
  strcat(exec_path,in_argv[0]);
  puts(exec_path);

  int ac = access(exec_path,X_OK);
  return ac;
}

int main() {
  int returncode = prompt();
  printf("%d\n",returncode);
  return 0;
}
