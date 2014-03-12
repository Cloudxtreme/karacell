#include "flag.h"
#include <stdint.h>
#include <stdlib.h>
#ifdef PTHREAD
  #include <pthread.h>
#endif
#include "constant.h"
#include "spawn.h"
#include "lmd7.h"
#include "karacell.h"
#include "karacell_xtrn.h"
#include "entropy_xtrn.h"
#include "listcrypt.c"
#include "u8crypt.c"
#include "u16crypt.c"
#include "u32crypt.c"
