/*
 *  UScanner - The Universal Scanner
 *  by Matt Baron
 *
 *  File: uscanner.c
 *
 *  Copyleft (c) 1997 under the GNU Public Licence (GPL)
 */

#include "uscanner.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TFORMAT "%s (%d)"

/** Prototypes ***/
void get_next_line(ScanRec*);
char next_char(ScanRec*);
char scan_for_env(ScanRec*);
void buffer_char(ScanRec*, char);
int s_get_next_token(ScanRec*);

/*** Public Functions ***/
ScanRec* NewScanner(char* filename, TokenDefRec *t, int ntokens) {
ScanRec *s=NULL;

   s = (ScanRec*)malloc(sizeof(ScanRec));

   if (s == NULL) return NULL;

   s->tokens = t;
   s->num_tokens = ntokens;
   s->line_num = 0;
   s->buff_index = -1;
   s->token_available = 0;
   s->error_flag = 0;
   strcpy(s->buff, "");

   if ((s->fp = fopen(filename, "r")) == NULL)
      return NULL;
 
   get_next_line(s);

   return s;
}

int ScannerDone(ScanRec *s) {
   return (fclose(s->fp));
}

int Match(ScanRec *s, int t) {

   if (t != NextToken(s) && t != Anything) {
      printf("line %d: expected ", s->line_num);
      DecodeToken(s, t);
      printf(" but found ");
      DecodeToken(s, s->ctoken);
      printf("\n");
      s->error_flag = 1;
      s->token_available = 0;
      return 0;
   }
   s->token_available = 0;
   return 1;
}

int NextToken(ScanRec *s) {
  if (!s->token_available) {
     s->ctoken = get_next_token(s);
     s->token_available = 1;
  }
  return s->ctoken;
}

int CaptureToken(ScanRec *s, char **str) {
   *str = (char*)malloc(strlen(s->token_buffer)*sizeof(char)+1);
   if (*str == NULL) return 0;
   strcpy(*str, s->token_buffer);
   return 1;
}

void DecodeToken(ScanRec *s, int token) {
int i;

   for (i = 0; i < s->num_tokens; i++)
      if (s->tokens[i].id == token) {
         printf(TFORMAT, s->tokens[i].def, s->tokens[i].id);
         return;
      }

    switch(token) {
       case LParen: printf(TFORMAT, "LParen", token); break;
       case RParen: printf(TFORMAT, "RParen", token); break;
       case LBrace: printf(TFORMAT, "LBrace", token); break;
       case RBrace: printf(TFORMAT, "RBrace", token); break;
       case EofSym: printf(TFORMAT, "End of File", token); break;
       case Id    : printf(TFORMAT, "Identifier", token); break;
       case Comma : printf(TFORMAT, "Comma", token); break;
       default    : printf(TFORMAT, "Unknown", token); break;
    }
}

void ScanId(ScanRec *s) {
char id_token[MAX_TOKEN];

   strcpy(id_token, "");

   while(NextToken(s) == Id) {
      sprintf(id_token, "%s %s", id_token, s->token_buffer);
      Match(s, Id);
   }

   strcpy(s->token_buffer, &id_token[1]);
}

/*** Private Functions ***/
void get_next_line(ScanRec *s) {
   s->buff_index = -1;
   s->line_num++;
   s->eof = (fgets(s->buff, MAX_LINE, s->fp) == NULL);
   s->buff[strlen(s->buff)] = '\0';
}

char next_char(ScanRec *s) {
   while (s->buff[s->buff_index+1] == '\0' || s->buff[s->buff_index+1] == COMM)
      get_next_line(s);

   return(s->buff[++s->buff_index]);
}

char peek(ScanRec *s) {
   if (s->buff[s->buff_index] == '\0')
      return '\0';
   else
      return s->buff[s->buff_index+1];
}

void buffer_char(ScanRec *s, char c) {
   s->token_buffer[s->token_index++] = c;
}

char scan_for_env(ScanRec *s) {
char *env_var = NULL;
char var[50];
char curr_char;
int index=0;

   next_char(s);
   curr_char = next_char(s);

   while(curr_char != ')' && curr_char != '\0') {
      var[index++] = curr_char;
      curr_char = next_char(s);
   }

   var[index] = '\0';

   if ((env_var = getenv(var)) != NULL) {
      strcpy(&s->token_buffer[s->token_index], env_var);
      s->token_index += strlen(env_var);
   }
   return next_char(s); 
}

int check_reserved(ScanRec *s) {
int i;

   for (i = 0; i < s->num_tokens; i++)
      if (s->token_buffer[0] == s->tokens[i].def[0] &&
          s->token_buffer[1] == s->tokens[i].def[1] &&
          !strcmp(s->token_buffer, s->tokens[i].def))
         return s->tokens[i].id;

   return Id;
}

int s_get_next_token(ScanRec *s) {
char curr_char;

   s->token_index = 0;
   strcpy(s->token_buffer, "");

   curr_char = next_char(s);

   while(isspace(curr_char) && !s->eof) curr_char = next_char(s);
   if(s->eof) return EofSym;

   switch(curr_char) {
      case '{': return LBrace;
      case '}': return RBrace;
      case ',': return Comma;
   }

   while(!isspace(curr_char) && !s->eof) {
      switch(curr_char) {
         case '{': 
         case '}':  
         case ',': break; 
      }

      if (curr_char == '$' && peek(s) == '(') 
         curr_char = scan_for_env(s);
      else {
         s->token_buffer[s->token_index++] = curr_char;
         curr_char = next_char(s);
      }
   }

   s->buff_index--;
   s->token_buffer[s->token_index++] = '\0';
   return (check_reserved(s));
}

int get_next_token(ScanRec *s) {
char curr_char;
int tok;

   s->token_index = 0;
   strcpy(s->token_buffer, "");

   while (!s->eof)  {
      curr_char = next_char(s);

      if(isspace(curr_char)) continue;
      if(s->eof) return EofSym;

      switch(curr_char) {
         case '{': return LBrace;
         case '}': return RBrace;
         case ',': return Comma;
      }

      if (curr_char == '"') {
         curr_char = next_char(s);
         while(curr_char != '"' && curr_char != '\0') {
            if (curr_char == '$' && peek(s) == '(') {
               curr_char = scan_for_env(s);
            } else {
  	       if (curr_char == '\\')
                  curr_char = next_char(s);
               buffer_char(s, curr_char);
               curr_char = next_char(s);
            }
         }
         buffer_char(s,'\0');
         return Id;
      }

      while(!isspace(curr_char) && curr_char != '}' &&  curr_char != '{' && curr_char != ',') {
         if (curr_char == '$' && peek(s) == '(') {
            curr_char = scan_for_env(s);
         } else {
            buffer_char(s,curr_char);
            curr_char = next_char(s);
         }
      }
      s->buff_index--;
      buffer_char(s,'\0');
      return (check_reserved(s));
   }
   return EofSym;  
}
