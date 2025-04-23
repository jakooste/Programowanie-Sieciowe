#include<stdio.h>

void drukuj(int tablica[], int liczba_elementow){
  for(int i=0; i<liczba_elementow; i++){
    if((tablica[i] > 10) && (tablica[i] < 100)){
      printf("%d ",tablica[i]);
    }
  }
  printf("\n");
}

void drukuj_alt(int* tablica, int liczba_elementow) {
  for(int i=0; i<liczba_elementow; i++) {
    if((*tablica > 10) && (*tablica < 100)) {
	printf("%d ", *tablica);
    }
    tablica++;
  }
  printf("\n");
}

int main() {
  int liczby[50];
  int ile_liczb=0;
  while(ile_liczb<50) {
    scanf("%i", &liczby[ile_liczb]);
    if(liczby[ile_liczb]==0) {break;}
    ile_liczb++;
  }
  drukuj_alt(liczby, ile_liczb);
  return 0;
}
