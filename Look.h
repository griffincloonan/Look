#define STRLEN 30
#define LINELEN 50
#define FLAG_D 4
#define FLAG_F 2
#define FLAG_T	1

int MyStrCmp(char *str1, char *str2, int length, int flags);

void FindString(char searchStr[], int flags, char termChar);

void ParseArgs(int argc, char *argv[], int *flags, char searchStr[], char fileName[], char *termChar);

void SortFile(char fileName[], int flags);