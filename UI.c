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
  for (int i = 0; i < countElements; i++) {
    printf("%d. %s\n", i+1, array[i]);
  }

  int choice = -1;
  do {
    printf("Enter a number from 1 to %d: ", countElements);
    scanf("%d", &choice);
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
  } while(choice < 1 || choice > countElements);

  return choice;
}

void formatToNumber(char *mobileNumber) {
  char *position = mobileNumber;
  char *digitPosition = mobileNumber;
  char ch;

   do {
    ch = *position;

    if (isdigit(ch)) {
      *digitPosition = ch;
      digitPosition++;
		}

		position++;
  } while (ch != '\0');

  *digitPosition = '\0';
}

char *askMobileNumber() {
  char * number = malloc(30);

  while(1) {
    fgets(number, 30, stdin);
    formatToNumber(number);

    if (strlen(number) > 0) {
      break;
    } else {
      printf("Number is incorrect. Try again:\n");
    }
  }

  return number;
}

void askEnter() {
  printf("To return to main menu press Enter...\n");
  int c;
  while ((c = getchar()) != '\n' && c != EOF);
  getchar();
  return;
}

#endif
