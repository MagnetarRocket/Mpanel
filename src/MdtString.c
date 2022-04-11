#include "MdtString.h"
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

int MdtVaString(char **target, ...) {
va_list args;
char *value;
int length=0;

   va_start(args, target);
   value = va_arg(args, char*);

   while(value != NULL) {
      length += strlen(value);
      value = va_arg(args, char*);
   }

   #ifdef _DEBUG
      printf("MdtVaString: allocating string (%d)\n", length);
   #endif

   *target = (char*)malloc(length+2*sizeof(char));

   if (*target == NULL) return 0;

   *target[0] = '\0';

   va_start(args, target);
   value = va_arg(args, char*);

   while(value != NULL) {
      strcat(*target, value);
      value = va_arg(args, char*);
   }
   return 1;
}

int MdtStringSet(char **str, char *value) {

   *str = (char*)malloc(strlen(value)+1*sizeof(char));

   if (*str == NULL) return 0;

   strcpy(*str, value);
   return 1;
}

int MdtStringSet2(char **str, char *value1, char *value2) {

   *str = (char*)malloc(strlen(value2)+strlen(value2)+1*sizeof(char));

   if (*str == NULL) return 0;

   sprintf(*str, "%s%s", value1, value2);

   return 1;
}

int MdtStringFree(char **str) {

   #ifdef _DEBUG
      puts("MdtStringFree: freeing string...");
   #endif

   if (*str != NULL)
      free(*str);

   *str = NULL;
   return 1;
}

int MdtStringEmpty(char *str) {
int i;

   for(i = 0; i < strlen(str); i++)
      if (!isspace(str[i]))
         return 0;
   return 1;
}
