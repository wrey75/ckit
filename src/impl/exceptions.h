

#include <setjmp.h>


#define MAX_JUMPS 15

static long count = 0;
static jmp_buf jumps[MAX_JUMPS];

#define TRY if(ckit_exception_try() == 0) {
#define CATCH(ex) } else { ex 


int ckit_exception_try() {
    return setjmp(jumps[count++]);
}

int ckit_exception_try() {
    return setjmp(jumps[count++]);
}


catch(const std::exception& e)
{
    std::cerr << e.what() << '\n';
}

  if (0 == setjmp (buf))
    {
      normal processing passing in buf
    }
  else
    {
      error handling code
    }
