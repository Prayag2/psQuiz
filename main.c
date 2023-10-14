#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <time.h>

#define MAX_STR_LENGTH 50
#define NAME_LENGTH 30
#define PRICE_MULTIPLIER 2000

// Structures
typedef struct {
  char name[NAME_LENGTH];
  unsigned int questionsCorrect;
  unsigned int balance;
} User;

typedef struct {
  char *question;
  char *options[4];
  int correctOptionIndex;
} Question;

typedef struct {
  int remaining;
} Lifeline;

// Functions
void multiplyChar(char character, int count, char *prefix, char *suffix) {
  /*
    This is a utility function which prints a character "count" times.
    Args:
        character: the character to be printed
	count: how many times to repeat it
	prefix: what to print before repeating
	suffix: what to print after repeating
   */
  printf("%s", prefix);
  for (int i = 0; i < count; i++)
    printf("%c", character);
  printf("%s", suffix);
}

unsigned long getMaxLength(char arr[][MAX_STR_LENGTH], unsigned long arrSize) {
  /*
    This is a utility function which returns length of the longest string in an array
    Args:
        arr: The array of strings
        arrSize: Size of the array since it cannot be calculated
   */
  unsigned long maxVal;
  for (unsigned long index = 0; index < arrSize; index++) {
    unsigned long stringLength = strlen(arr[index]);
    if (index == 0 || stringLength > maxVal)
      maxVal = stringLength;
  }
  return maxVal;
}

void decorateText(char text[][MAX_STR_LENGTH], unsigned long arrSize) {
  /*
    A utility function which creates a one column table using an array of strings.
    Args:
        text[]: array of text to be printed
	arrSize: size of text[]
   */
  for (unsigned long arrIndex = 0; arrIndex < arrSize; arrIndex++) {
    unsigned long repeatCount = getMaxLength(text, arrSize) + 2;
    if (strcmp(text[arrIndex], "")) {
      multiplyChar('-', repeatCount, "+", "+\n");
      printf("| %s", text[arrIndex]);
      multiplyChar(' ', repeatCount - 2 - strlen(text[arrIndex]), "", " |\n");
    }
    if (arrIndex == arrSize-1)
      multiplyChar('-', repeatCount, "+", "+\n");
  }
}

void getCharInput(char *prompt, char *userInput) {
  /*
    A utility function which takes user input and returns lowercase version of it.
    Args:
        prompt: The prompt text
	userInput: pointer to userInput
   */
  printf("%s", prompt);
  scanf("\n%c", userInput);
  *userInput = tolower(*userInput);
}

void useAudiencePole(Question question, char listCharacters[], Lifeline *audiencePole) {
  if (audiencePole->remaining == 0) {
    printf("Already used!");
    return;
  }
  printf("The audience is now voting...\n");
  sleep(2);

  char results[][MAX_STR_LENGTH] = {
    "Audience Pole Results",
    "",
    "",
    "",
    "",
  };
  srand(time(0));
  int percentage;
  // 1 in 100 chance to get wrong answer from the audience
  bool willShowCorrect = (rand()%100) == 50 ? false : true; 
  for (int i = 0; i < 4; i++) {
    if (i == question.correctOptionIndex) {
      printf("willShowCorrect %d\n", willShowCorrect);
      percentage = willShowCorrect ? rand()%10+90 : rand()%10;
    } else {
      percentage = willShowCorrect ? rand()%10 : rand()%50;
    }

    sprintf(results[i+1], "%c: %d%%", listCharacters[i], percentage);
  }
  decorateText(results, 5);
  audiencePole->remaining -= 1;
}

void useFiftyFifty(Question question, char listCharacters[], Lifeline *fiftyFifty) {
  if (fiftyFifty->remaining == 0) {
    printf("Already used!");
    return;
  }
  int wrongOptions[2], i = 0;
  srand(time(0));
  while (i < 2) {
    int num = rand()%3;
    if ((num == question.correctOptionIndex) || (i == 1 && wrongOptions[0] == num)) {
      continue;
    }
    wrongOptions[i] = num;
    i++;
  }
  printf("The two wrong options are %c and %c\n", listCharacters[wrongOptions[0]], listCharacters[wrongOptions[1]]);
  fiftyFifty->remaining -= 1;
}

void useLifeline(Question question, char *listCharacters, Lifeline *audiencePole, Lifeline *fiftyFifty) {
  char userInput;

  while (true) {
    getCharInput("Which lifeline? (A/F/Q to quit): ", &userInput);
    switch(userInput) {
      case 'a':
	useAudiencePole(question, listCharacters, audiencePole);
	return;
      case 'f':
	useFiftyFifty(question, listCharacters, fiftyFifty);
	return;
      case 'q':
	return;
      default:
	printf("Invalid input\n");
    }
  }
}

bool askQuestion(Question question, char listCharacters[], Lifeline *audiencePole, Lifeline *fiftyFifty) {
  char userInput;

  // Asking The Question
  printf("Q: %s\n", question.question);
  for (int optionIndex = 0; optionIndex < 4; optionIndex++) {
    printf("(%c) %s\n", listCharacters[optionIndex], question.options[optionIndex]);
  }

  while (true) {
    getCharInput("Your Input: (L/A/B/C/D/Q to quit): ", &userInput);
    if (userInput == 'l') {
      useLifeline(question, listCharacters, audiencePole, fiftyFifty);
      continue;
    } else if (userInput == 'q') {
      printf("Thanks for playing!\n");
      exit(0);
    }
    break;
  }
  if (userInput == tolower(listCharacters[question.correctOptionIndex]))
    return true;
  else {
    return false;
  }
}

void setHud(char hudText[][MAX_STR_LENGTH], int balance, Lifeline *audiencePole, Lifeline *fiftyFifty) {
  sprintf(hudText[0], "Balance - Rs. %d", balance);
  if (audiencePole->remaining > 0) {
    sprintf(hudText[1], "Audience Pole (%d)", audiencePole->remaining);
  } else {
    strcpy(hudText[1], "");
  }
  if (fiftyFifty->remaining > 0) {
    sprintf(hudText[2], "Fifty Fifty (%d)", fiftyFifty->remaining);
  } else {
    strcpy(hudText[2], "");
  }
}

// Main
int main() {
  // Declarations
  char listCharacters[4] = {'A', 'B', 'C', 'D'};
  char hudText[][MAX_STR_LENGTH] = {"", "", ""};

  User user = {"", 0, 0};
  Lifeline fiftyFifty = {1}, audiencePole={1};
  Question questions[] = {
    {"Who is the prime minister of India?", {"Kajribal", "Modi", "Rahul Gandhi", "Sardar"}, 1},
    {"Who created Linux", {"Linus Tech Tips", "Linus Torvalds", "Linus Doe", "The Linux Company"}, 1},
    {"Who created Windows", {"Bill Windows", "Bill Trapdoors", "Bill Curtains", "Bill Gates"}, 3},
    {"What is Ligma?", {"A leg disease", "A balls disease", "Ligma balls", "An STD"}, 2},
    {"Who is Joe?", {"Joe Mama", "John Doe's Brother", "Joe", "Peter Parker"}, 0},
  };

  printf("Enter full name: ");
  fgets(user.name, NAME_LENGTH, stdin);

  for (int i = 0; i < 5; i++) {
    system("clear");
    setHud(hudText, user.balance, &audiencePole, &fiftyFifty);
    decorateText(hudText, 3);
    if(askQuestion(questions[i], listCharacters, &audiencePole, &fiftyFifty)) {
      user.balance += (i+1)*PRICE_MULTIPLIER;
      user.questionsCorrect++;
    } else {
      printf("Game Over!\n");
      exit(0);
    }
  }

  return 0;
}
