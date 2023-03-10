#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>

int iswhitespace(char c);
int prompt();
int parse_cmd(char* buffer);
int execute(int argc, char** argv);
int execute_builtin(int argc, char** argv);


void print_err();



char path[1000] = "/usr/bin /bin";

char exit_code[] = "exit";
char cd_code[] = "cd";
char path_code[] = "path";
char error_message[] = "An error has occurred\n";
int errmsg_len = 30;

size_t lim = 1000; //character limit of commands

void print_err(){
    write(STDERR_FILENO, error_message, strlen(error_message)); 
}
int iswhitespace(char c){
  return (c == 32) || (c == 9) || (c == 10) || (c==0);
}

int main(int argc, char* argv[]) {
  //prompts the user for a command. Parses it, and then executes it

  //if an argument has been passed, enter batch mode
  int interactive=(argc==1);
  if (argc>1){
    close(STDIN_FILENO);
    open(argv[1],O_RDONLY);
  }
  int rp;

  while (1){
    prompt();
  }
}

int prompt() {
    char* buffer = NULL;
    printf("wish> ");
    int chars = getline(&buffer,&lim,stdin);

    int i = 0;
    int n_cmd = 1;
    while (buffer[i]!='\n'){
        if (buffer[i]=='&') n_cmd++;
        i++; 
    }
    int pids[n_cmd];

    char multi_delim[] = "&";
    char* context;
    char* tok = strtok_r(buffer,multi_delim,&context);
    i = 0;

    while (tok != NULL){
      char* cmd = strdup(tok);
      pids[i++] = parse_cmd(cmd);
      tok = strtok_r(NULL,multi_delim,&context);
    }
    
    i = 0;
    int* status;
    while(i<n_cmd){
        if (pids[i]>0){
            waitpid(pids[i],status,0);
        }
        i++;
    };
    return 0;
}

int parse_cmd(char* buffer){
    //returns PID of parsed command
    char delim[]= " \n\t";

    int argc = 1;
    int i = 1;
    //count number of arguments
    while (buffer[i]!='\0'){
        if (iswhitespace(buffer[i-1]) && !iswhitespace(buffer[i])) {
            argc+=1;
        }
        i++;
    }

    char* argv[argc+1];
    argv[0] = strtok(buffer,delim);
    i = 1;
    while((argv[i++]=strtok(NULL,delim))!=NULL);

    return execute(argc,argv);
}

int execute(int argc, char** argv){
    //returns PID of child process
    //if built-in command or no command, return -1
    if (execute_builtin(argc,argv)==0){
        return -1;
    }

    char exec_path[100];
    char* search_path;
    int ac;
    char* path_cpy = strdup(path); //we don't want to add null characters into the path
    //so we copy it in order to perform parsing
    search_path = strtok(path_cpy," ");
    if (search_path==NULL) {
      print_err();
      return -1;
    }
    do {
        strcpy(exec_path,search_path);
        strcat(exec_path,"/");
        strcat(exec_path,argv[0]);
        ac = access(exec_path,X_OK);
        if (ac == 0) break; 
    } while ((search_path = strtok(NULL," ")) != NULL);

    if (ac != 0) {
        print_err();
        return -1; // couldn't find it :(
    }

    argv[0] = exec_path;

    int rc = fork();
    if (rc < 0) exit(-1);
    else if (rc == 0) {
    execv(argv[0],argv);
    } else if (rc > 0) {
    return rc;
    }

}

int execute_builtin(int argc, char** argv){
    //return -1: no bultin was executed, search PATH
    //return 0: builtin was executed, continue to next command

    //exit
    if (strcmp(argv[0],exit_code)==0){
        if (argc!=1) {
          print_err();
          return 0;
        }
        exit(0);
      }
      //cd
      if (strcmp(argv[0],cd_code)==0){
        if (argc!=2) {
          print_err();
          return 0;
        }
        chdir(argv[1]);
        return 0;
      }
      //path
      char space[] = " ";
      if (strcmp(argv[0],path_code)==0){
        path[0] = '\0';
        for (int i = 1; i<argc; i++){
          strcat(path,argv[i]);
          strcat(path,space);
        }
        return 0;
      }
    return -1;         
}
