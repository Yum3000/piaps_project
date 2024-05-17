#ifndef UI_C
#define UI_C

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

void clear(){
    #if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
        system("clear");
    #endif

    #if defined(_WIN32) || defined(_WIN64)
        system("cls");
    #endif
}

char *askString(char *string) {
  printf("%s\n", string);
  
  char *answer = malloc(100);
  scanf("%99s", answer);

  return answer;
}

int askNumber(char *string){
  printf("%s\n", string);
  int answerNumber = 0;
  scanf("%99d", &answerNumber);
  // исправить ограничение на string
  return answerNumber;
}

int askVariant(char **array, int countElements) {
  int choice = 0;
  for (int i = 0; i < countElements; i++) {
    printf("%d. %s\n", i+1, array[i]);
  }
  scanf("%d", &choice);

  return choice;
}

void askAnyKey() {
  char *anyKey = malloc(5);
  printf("To return to main menu press any key...\n");
  //gets(anyKey);
  scanf("%5s", anyKey);
  // очищать буфер??
  //getchar(); // wait for ENTER
  //getc(stdin);
  printf("ok...\n");
  return;
}

char *formatNumber(char *mobileNumber) {
	
  char *currPointer = mobileNumber;
  char *toSave = mobileNumber;
  
  char ch = *currPointer;
  
  while (ch != '\0') {
	    ch = *currPointer;
	    if (isdigit(ch)) {
			*toSave = ch;
			toSave++;
		}
		currPointer++;
  }
  
  *toSave = '\0';

  return mobileNumber;
}
#endif
