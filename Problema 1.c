/*  Din Teme de Laborator/Fisiere, directoare
  ENUNT: 
5. Scrieţi un program care determină dacă conţinutul unui fişier este inclus
(contiguu) ı̂n conţinutul altui fişier şi de câte ori (numărul apariţiilor). Se vor
considera şi apariţiile care se suprapun. Specificatorii fişierelor sunt daţi ca
argumente ı̂n linia de comandă. Se vor folosi funcţii de nivel superior (tipul
’FILE’).
*/

#include <stdio.h>
#include <stdlib.h>
#include "UI/cssfrm.h"

void Eroare(const char *motiv) {
  perror(motiv); 
  exit(1);
}
void FRError(int arg) {
  printf("Eroare la un fread (%d)!", arg);
  exit(1);
}

int main(int nrArg, char *Arg[]) {
/*int main() {
  char *Arg[] = {NULL,"Samples.txt", "DeCautat.txt"};
/*printf("%s", Arg[1]);
  printf("\n%s\n", Arg[2]);
  exit(1);
*/

  FILE *SamplesFL = fopen(Arg[1], "r"); 
    if(SamplesFL == NULL) Eroare(Arg[1]);
      fseek(SamplesFL, 0, SEEK_SET);
  FILE *DeCautatFL = fopen(Arg[2], "r");
    if(DeCautatFL == NULL) Eroare(Arg[2]);
  
  int sizeSample = 0;
  fseek(SamplesFL, 0, SEEK_END);
  sizeSample = ftell(SamplesFL);
  fseek(SamplesFL, 0, SEEK_SET);

  int sizeText = 0;
  fseek(DeCautatFL, 0, SEEK_END);
  sizeText = ftell(DeCautatFL);
  fseek(DeCautatFL, 0, SEEK_SET);

  if(sizeSample == 0) {
    printf("Primul fisier e gol, nr de aparitii = 0!\n");
    return -1;
  }

  int nrAparitii = 0;
  char Sbuff, Tbuff, Firstbuff;
  if(fread(&Firstbuff, sizeof(char), 1, SamplesFL) != 1) FRError(1);
    for(int i = 0; i < sizeText - sizeSample + 1; i++) {
      fseek(DeCautatFL, i, SEEK_SET);
      if(fread(&Tbuff, sizeof(char), 1, DeCautatFL) != 1) FRError(2);

      if (Tbuff != Firstbuff) continue;

      fseek(SamplesFL, 1, SEEK_SET);
      for(int j = 1; j < sizeSample; j++) {
        if(fread(&Tbuff, sizeof(char), 1, DeCautatFL) != 1) FRError(3);
        if(fread(&Sbuff, sizeof(char), 1, SamplesFL) != 1) FRError(4);

        if(Tbuff != Sbuff) {
          break;
        }
        if(j == sizeSample - 1) {  
          nrAparitii++;
        }
      }
    }

  free(SamplesFL);
  fclose(DeCautatFL);

    Format("#c#B", "cyn", "y");
  printf("\nTextul din primul fisier apare de ");
    Format("#c#B#i", "red", "y", "y");
  printf("%d ", nrAparitii);
    Format("#c#B", "cyn", "y");
  printf("ori in al doilea.\n\n");
    Format("");

return 0;
}

// fal sa dea printf la erori de memorie    X
// sa se compileze bine in consola          X
//schimba din.c in .h                       X
// fai .sh                                  X
// fal rulabil de oriunde cu sh             .
// chmod +x la .sh                          x
// perror                                   X
// fa cu fseekeuri la fisieru mare ca sa nu bagin in heap          X