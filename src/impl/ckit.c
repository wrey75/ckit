
#include <stdio.h>
#include <stdlib.h>
#include <execinfo.h>
#include "../ckit.h"
q
/**
 * Initialize the kit.
 * 
 */
void ckit_init(void) {

}

void ckit_exit(const char *format, ...){
    ckit_backtrace();                                  

                 char message[255];                                                        
   va_list arg_ptr;                                                             
   va_start(arg_ptr, format);                                                      
   vsprintf(message, sizeof(255), format, arg_ptr);                                              
   va_end(arg_ptr);                                                             
          
    fprintf(stderr, "**FATAL**: %s\n", message);
    exit(1);
}


  void ckit_backtrace(void)
       {
           int nptrs;
           void *buffer[100];
           char **strings;

           nptrs = backtrace(buffer, sizeof(buffer));
           fprintf(stderr, "backtrace() returned %d addresses:\n", nptrs);

           /* The call backtrace_symbols_fd(buffer, nptrs, STDOUT_FILENO)
              would produce similar output to the following: */

           strings = backtrace_symbols(buffer, nptrs);
           if (strings == NULL) {
               perror("backtrace_symbols");
               exit(EXIT_FAILURE);
           }

           for (int j = 0; j < nptrs; j++)
               fprintf(stderr, "%s\n", strings[j]);

           free(strings);
       }



