#include "Look.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <unistd.h> 
#include <ctype.h>

int main(int argc, char *argv[]) {
	//Bits for flags: d|f|t
	int flags = 0;

	char searchStr[STRLEN] = {0};
	char fileName[STRLEN] = {0};
	char termChar = '\0';

	//Check for proper number of args
	if (argc <= 1 || argc > 6) {
		printf("usage: look [-df] [-t char] string [file ...]\n");
		exit(1);
	}

	ParseArgs(argc, argv, &flags, searchStr, fileName, &termChar);

	if (strlen(fileName) > 0) {
		if (open(fileName, O_RDONLY) == -1) {
			printf("file open failed\n");
			exit(1);
		}

		SortFile(fileName, flags);
	}
	else {
		if (open("/usr/share/dict/words", O_RDONLY) == -1) {
			printf("file open dictionary failed\n");
		}

		SortFile("/usr/share/dict/words", flags); 
	}

	FindString(searchStr, flags, termChar);

	return 0;

}

void FindString(char searchStr[], int flags,  char termChar) {
	int curLineNdx = 0, length = 0, curChar = 0;
	char toPrint[LINELEN] = {0};
	FILE *srtFile; 

   if ((srtFile = fopen("sortedFile", "r")) == NULL) {
		printf("fopen sortedFile failed\n");
		exit(1);
   }

   //Check for -t flag: check searchStr up to termChar
	if (flags & FLAG_T) {
		//If -f flag ignore case of termChar
		if (flags & FLAG_F) {
			while (curChar <= strlen(searchStr) && (tolower(searchStr[curChar]) != tolower(termChar))) {
				curChar++;
			}
		}
		else {
			while (curChar <= strlen(searchStr) && searchStr[curChar] != termChar) {
				curChar++;
			}
		}

		length = curChar + 1;
	}
	else {
		length = strlen(searchStr);
	}

	//Parse sortedFile and print out lines beginning with searchStr
	while (fgets(toPrint, LINELEN, srtFile) != NULL) {
		//Remove trailing return character
		if (toPrint[strlen(toPrint) - 1] == '\n') {
			toPrint[strlen(toPrint) - 1] = '\0';
		}
		
		if (MyStrCmp(searchStr, toPrint, length, flags) == 0) {
			printf("%s\n", toPrint);
		}

   	curLineNdx++;
	}

}

int MyStrCmp(char *str1, char *str2, int length, int flags) {
	
	//If -f flag: ignore case when comparing
	if (flags & FLAG_F) {

		//If -d flag: skip non-alphanumeric characters
		if (flags & FLAG_D) {
			while (!isalnum(*str1)) {
				str1++;
			}
			while (!isalnum(*str2)) {
				str2++;
			}
		}

		while (tolower(*str1) == tolower(*str2) && --length) {
			str1++;
			str2++;

			//If -d flag: skip non-alphanumeric characters
			if (flags & FLAG_D) {
				while (!isalnum(*str1)) {
					str1++;
				}
				while (!isalnum(*str2)) {
					str2++;
				}
			}
		}

		return tolower(*str1) - tolower(*str2);
	}
	//Else no -f flag: do not ignore case when comparing
	else {

		//If -d flag: skip non-alphanumeric characters
		if (flags & FLAG_D) {
			while (!isalnum(*str1)) {
				str1++;
			}
			while (!isalnum(*str2)) {
				str2++;
			}
		}

		while (*str1 == *str2 && --length) {
			str1++; 
			str2++;

			//If -d flag: skip non-alphanumeric characters
			if (flags & FLAG_D) {
				while (!isalnum(*str1)) {
					str1++;
				}
				while (!isalnum(*str2)) {
					str2++;
				}
			}
		}

		return *str1 - *str2;
	}
}

void SortFile(char fileName[], int flags) {
	int sortedFile;

	//Create new file to output 'sort' to
	if ((sortedFile = open("sortedFile", O_RDWR | O_CREAT | O_TRUNC, 0600)) == -1) {
		printf("sortedFile open failed\n");
		exit(1);
	}

	//Redirect stdout to sortedFile and exec sort
	if (fork() == 0) {
		dup2(sortedFile, 1);

		//If -d flag then file is sorted in dictionary order (alphanumerics compared only)
		if (flags & FLAG_D) {
			execlp("/usr/bin/sort", "/usr/bin/sort", "-d", fileName, NULL);
			printf("exec failed\n");
		}
		else {
			execlp("/usr/bin/sort", "/usr/bin/sort", fileName, NULL);
			printf("exec failed\n");
		}
	}
	else {
		wait(NULL);
	}
}

void ParseArgs(int argc, char *argv[], int *flags, char searchStr[], char fileName[], char *termChar){
	int curArg;
	
	for (curArg = 1; curArg < argc; curArg++) {
		//Check for flag
		if (*argv[curArg] == '-') {
			//Check which flag and 'or' it into flags variable
			while (*(++argv[curArg]) != '\0') {
				if (*(argv[curArg]) == 'd') {
					*flags |= FLAG_D;
				}
				else if (*(argv[curArg]) == 'f') {
					*flags |= FLAG_F;
				}
				else if (*(argv[curArg]) == 't') {
					*flags |= FLAG_T;

					curArg++;
					*termChar = *(argv[curArg]);
				}
				else {
					printf("look: illegal option -%c\n", *(argv[curArg]));
					exit(1);
				}
			}
		}
		else if (strlen(searchStr) == 0) {
			strcpy(searchStr, argv[curArg]);
		}
		else {
			strcpy(fileName, argv[curArg]);
		}
	}
}