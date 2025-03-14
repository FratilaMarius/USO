/*In linux, fara system(): Implementaţi un shell cu următoarele facilităţi: 
  - acceptă comanda internă ’exit’; 
  - acceptă comenzi externe cu un număr oarecare de argumente; 
  - acceptă rularea de script-uri; script-ul poate conţine o listă de comenzi 
    utilitare şi poate accepta argumente ı̂n linia de comandă.
*/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/resource.h>
  #include "UI/cssfrm.h"

  #define NR_MAX_CHAR_PERLINE 1024
  #define MAX_ARG_LEN 128
  #define NR_MAX_ARG 32

    #define COLR_HIGHLIGH "grn"
    #define COLR_ERROR "red"
    #define COLR_PATH "blu"
void PrintInstr() {
  Format("#c#i", "cyn", "y");
  printf("BDNSH accepta:\n  -comenzi externe\n  -scripturi simple(fara if, while, ...), cate o comanda pe rand, cu extensia .sh\n  -'cd', cu argument folderul\n  -se poate accesa history-ul comenzilor, 5 comenzi inapoi, sintaxa '>' + numarul, cu cate comenzi inapoi\n  -comanda 'exit' pentru a inchide programul\n		 BDNSH ARE PERMISIUNE LA SUDO!\n");
  Format("");
}
void PrintDelimitor() {
  Format("#c#B", COLR_HIGHLIGH, "y");
  printf("===========================================================================================================\n\n");
  Format("");
}
void PrintError(char *text) {
    Format("#b#i", COLR_ERROR, "y");
    printf("%s", text);
    Format("");
    printf("\n");
  }
int IsSHScript(char *arg) {
    int len = strlen(arg);
    if(arg[len-3] == 46 && arg[len-2] == 115 && arg[len-1] == 104) return 1; // daca are .sh
    //if(arg[0] == 46 || arg[0] == 47) return 1;
    //^daca vreau sa ma smecheresc sa il iau dupa ./ e complicat ca ce ii pasez aci
    // tre sa fie si numele fisierului, ar trebui ras adaosu intai
    return 0;
  }

int Parse(char *input, char **arg){
  int i = 0, a = 0, b = 0;
  while(1) {
    if(arg[a] == NULL) arg[a] = malloc(MAX_ARG_LEN); // il alochez cand intru intrun arg nou
      if(arg[a] == NULL) {
        perror("Erroare la malloc!");
        return -3;
      }
      if(b > MAX_ARG_LEN) { // daca un arg e prea mare
        PrintError("Max argument lenght exceded!");
        return -1;
      }
      if(a >= NR_MAX_ARG) { // daca am dat prea multe arg
        PrintError("Max argument number exceded!");
        return -2;
      }
    if(input[i] == 32) { // 32 == space   
      arg[a][b] = 0;
      arg[a] = realloc(arg[a], b+1);
      if(arg[a] == NULL) {
        perror("Erroare la realloc!");
        return -4;
      }
      b = 0;
      a++;
      i++;
      continue;
    }
    if(input[i] == 10) { // 10 == "\n"
      arg[a][b] = 0;
      arg[a] = realloc(arg[a], b+1);
      if(arg[a] == NULL) {
        perror("Erroare la realloc!");
        return -4;
      }
      b = 0;
      break;
    }
    arg[a][b] = input[i];
    i++;
    b++;
  }
  return a;
}

void ExecScript(char **arg) {
  int index = 1; // pentru argumentele pasate scriptului
   FILE *script = fopen(arg[0], "r");
     if(script == NULL) {
       PrintError("Fisier/Script nerecunoscut: nu a putut fi deshis");
       return;
     }
    fseek(script, 0 , SEEK_SET);
  char *scriptArgs[NR_MAX_ARG];
    for(int i = 0; i < NR_MAX_ARG; i++) {
      scriptArgs[i] = NULL;
    }

  char linieDinScript[NR_MAX_CHAR_PERLINE];
  while(fgets(linieDinScript, NR_MAX_CHAR_PERLINE, script)) {
    if(linieDinScript[0] == '\n' || linieDinScript[0] == '#') continue;
    
    // 
    int x = Parse(linieDinScript, scriptArgs);
      if(x == -1 || x == -2) {
          PrintError("(In script.sh)");
          fclose(script);
          for(int i = 0; i < NR_MAX_ARG; i++) {
          free(scriptArgs[i]);
        }
        return;
      }
    for(int i = 0; scriptArgs[i] != NULL; i++) {
      if(scriptArgs[i][0] == '$') {
        if(arg[index] == NULL) {
          PrintError("Nu au fost date destule argumente in linia de comanda pentru a rula scriptul!");
          fclose(script);
          for(int i = 0; i < NR_MAX_ARG; i++) {
            free(scriptArgs[i]);
          }
        return;
        }
        scriptArgs[i] = realloc(scriptArgs[i], strlen(arg[index]) + 1);
          if(scriptArgs[i] == NULL) {
            perror("Erroare la realloc");
            fclose(script);
            for(int i = 0; i < NR_MAX_ARG; i++) {
              if(scriptArgs[i] != NULL) free(scriptArgs[i]);
            }           
            return;
          }
        strcpy(scriptArgs[i], arg[index]);
        scriptArgs[i][strlen(arg[index])] = 0;
      }
    }
    // acum am in scriptArgs pe poz 0 comanda si pe urmatoarele argumentele comenzii, pasate prin shell
      // deci rulez:
    int pid = fork(); // pidu lu child
    if(pid == 0) {
      execvp(scriptArgs[0], scriptArgs);
        PrintError("Nu au fost date destule argumente in linia de comanda pentru a rula scriptul!");
        fclose(script);
        for(int i = 0; i < NR_MAX_ARG; i++) {
          free(scriptArgs[i]);
        }
      exit(-1);
    }
    else if(pid > 0) {
      //setpriority(PRIO_PROCESS, 0, 1);
      wait(NULL);
      //setpriority(PRIO_PROCESS, 0, 0);
    }
    else {
      PrintError("Nu s-a putut clona procesul pentru a executa scriptul!");
      perror(" Fatal. Reason");
        fclose(script);
      for(int i = 0; i < NR_MAX_ARG; i++) {
        free(scriptArgs[i]);
      }
      return;
    }
    for(int i = 0; i < NR_MAX_ARG; i++) {
      scriptArgs[i] = NULL;
    }
  }
  
fclose(script);
for(int i = 0; i < NR_MAX_ARG; i++) {
  free(scriptArgs[i]);
}
}

int main() {
  PrintDelimitor();
  //FILE *history = fopen("Log.bin", "w"); nvm prea complicat cu un log file, plus ca ocupa spatiu mult
  char history[5][NR_MAX_CHAR_PERLINE];
  int historyFlags[5];
    int indexHistory = 0;
  char input[NR_MAX_CHAR_PERLINE];
  char *arg[NR_MAX_ARG + 1];
  int iteration = 0;
  PrintInstr();
  while(1) {
    //==========================================================================================_MENU_
      // FLUFF: imi iau pwd ca sa il printez inainte de "Shell>", e convenabil cand il folosesc:
    char CWD[NR_MAX_CHAR_PERLINE];
    getcwd(CWD, NR_MAX_CHAR_PERLINE);

      for(int i = 0; i < NR_MAX_ARG + 1; i++) arg[i] = NULL;
    iteration++;
      Format("#c#B", COLR_PATH, "y");
    printf("\n~%s: ", CWD);
      Format("#c#B", COLR_HIGHLIGH, "y");
    printf("BDNSH> ");
      Format("");
    fgets(input, NR_MAX_CHAR_PERLINE, stdin);

    if(input[0] != '>') {
      strcpy(history[4], history[3]);
      strcpy(history[3], history[2]);
      strcpy(history[2], history[1]);
      strcpy(history[1], history[0]);
      strcpy(history[0], input);
      indexHistory++; historyFlags[indexHistory-1] = 1;
    }
    //strcpy(history[iteration-1], input);
    // print in history:
  //=============================================================================================
    if(input[0] == '\n')continue;
    // history:
      if(input[0] == '>' && input[2] == '\n' && input[1] <= 53 && input[1] > 48) { // inseamna ca a dat o comanda de history
        int cmdID = input[1] - 48;
        if(historyFlags[cmdID-1]) {
          printf("Executing: %s", history[cmdID-1]);
          strcpy(input, history[cmdID-1]);
        }
        else {
          PrintError("No such command exists!");
          continue;
        }
      }
  // exit urile:
    if( !strncmp(input, "exit", 4) || !strncmp(input, "EXIT", 4) || !strncmp(input, "Exit", 4)) {
      for(int i = 0; i < NR_MAX_ARG; i++) {
        if(arg[i] != NULL && iteration != 1) {
          if(arg[i] != NULL)
            free(arg[i]);
        }
      } 
      PrintDelimitor();
      //free(history);
      return 0;
    }
  // impartim in cuvinte:
      for(int i = 0; i < NR_MAX_ARG; i++) arg[i] = NULL;
    int nrArgs = Parse(input, arg);
  // error handleing: 
    if(nrArgs == -1 || nrArgs == -2) {
      for(int i = 0; i < NR_MAX_ARG; i++) {
        if(arg[i] != NULL)
          free(arg[i]);
      }
      continue;
    }
    // daca ma apuc sa ii dau spatii degeaba imi face arg goale in comada:
    nrArgs++;
    for(int i = 0; i < NR_MAX_ARG; i++) 
      if(arg[i] != NULL)
        if(arg[i][0] == 0) {
          arg[i] = NULL;
          nrArgs--;
        }
      // daca am avut asa ceva ajunge sa aiba argumentele separate de adrese nule, reasez:
      int cnt = 0;
        for(int i = 0; i < NR_MAX_ARG; i++) {
          if(arg[i] == NULL) continue;


          if(i != cnt) {
            if(arg[cnt] == NULL) arg[cnt] = malloc(MAX_ARG_LEN);
              if(arg[cnt] == NULL) {
                perror("Erroare la malloc in main (1)");
              }
            strcpy(arg[cnt], arg[i]);
            free(arg[i]);
            arg[i] = NULL;
          }
          cnt++;
          if(cnt >= nrArgs) break;
        }

  // daca a mers tot pana aici:
    //verific daca e script:
      if(IsSHScript(arg[0])) {
        ExecScript(arg);
        continue;
      }

    // suport pentru CD:
      if((arg[0][0] == 'c' && arg[0][1] == 'd')) {
        if(arg[1] != NULL) {
          if(chdir(arg[1])) {
            PrintError("Chdir failed ");
            perror("Reason");
          }
          continue;
        }
        else {
          PrintError("cd nu a prmit vreun argument");
          continue;
        }
      }
      if(arg[1] != NULL)
        if((arg[1][0] == 'c' && arg[1][1] == 'd')) {
          Format("#c#B", "yel", "y");
          printf("\nNo permisiion to use '%s' with cd!\n", arg[0]);
          Format("");
          if (arg[2] != NULL) {
            if(chdir(arg[2])) {

              PrintError("Chdir failed ");
              perror("Reason");
            }
          continue;
          }
          else {
            PrintError("cd nu a prmit vreun argument");
            continue;
          }
        }
    // ramane ca e comanda normala(daca nu e nici daia oricum da eroare linux si plange, nu ii fac error hdl):
      int pid = fork();
    if (pid == 0) {
        // copil
        execvp(arg[0], arg);
        PrintError("Comanda nerecunoscuta ");
        perror("Reason");
        exit(1);
    } else if (pid > 0) {
        wait(NULL);
        continue;
    } else {
      PrintError("Nu a iesit forkul ");
      perror("Fatal. Reason");
    }
  }// while

  for(int i = 0; i < NR_MAX_ARG; i++) {
    if(arg[i] != NULL)
      free(arg[i]);
  }
  //free(history);
  return 0;
}

// support pentru cd                                                                                            X
// suport pentru console running                                                                                X
// .sh settup file                                                                                              X
// UI ca de shell cu get cwd                                                                                    X
// sa fac ceva cu #define urile de culori                                                                       X
// refine si restrict la comenzi gen cd ca sa nu dea segfaut                                                    .
// print la instructions, ontrolls si ce paote sa faca BDNSH cand intra in el prima data                        .
// sa poata sa dea o comanda inapoi ^<> (cu un log file ceva, .bin daca am incredere de sine)                   x
  //^^^
    // deci trb sa stochez input intrun array de input
    // sal display cand apas o tasta (trb sa imi iau input instant fara enter ????kms)
    // daca apas alata tasta sau ajung la inceput trb sa sterg tot, mut cursoru in stdout la inceput si ii dau cu ecapecodu de sters linie
    // cand am gasit comanda din history de rulat trg sa ma apuc sa ii dau lu input valoarea ei, astfel cand detectez un enteer sa o ruleze bine(???????)
