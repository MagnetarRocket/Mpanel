#ifndef USCANNER_H
#define USCANNER_H

#include <stdio.h>

#define MAX_LINE 256
#define MAX_TOKEN 100
#define COMM '!'

typedef struct _TokenDefRec {
   int id;
   char *def;
} TokenDefRec;

typedef struct _ScanRec {
   FILE *fp;
   short token_available;
   char buff[MAX_LINE];
   char token_buffer[MAX_TOKEN];
   int ctoken;
   int buff_index, token_index, line_num;
   short eof, error_flag;

   int num_tokens;
   TokenDefRec *tokens;

} ScanRec;

#define LParen -1
#define RParen -2
#define LBrace -3
#define RBrace -4
#define Id -5
#define Comma -6
#define Anything -7
#define EofSym -8
#define Equal -9

ScanRec* NewScanner(char*, TokenDefRec*, int);
int ScannerDone(ScanRec *s);

void DecodeToken(ScanRec*,int);

int NextToken(ScanRec*);
int Match(ScanRec*, int);
int CaptureToken(ScanRec*, char**);
void ScanId(ScanRec*);
char* CaptureTokenR(ScanRec*);

#endif
