#include "flag.h"
#include <stdint.h>
#include <stdlib.h>
#ifdef PTHREAD
  #include <pthread.h>
#endif
#include "constant.h"
#include "simulthread.h"
#include "debug.h"
#include "spawn.h"
#include "lmd7.h"
#include "karacell.h"
#include "karacell_xtrn.h"
#include "spawn_xtrn.h"
#ifdef JYTTER
  #include "jytter.h"
#endif
#include "trng.c"
#include "entropy.c"
