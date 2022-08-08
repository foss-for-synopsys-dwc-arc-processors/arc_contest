#include <time.h>
#include <sys/time.h>
#include "SC16IS750_Bluepacket.h"
#define TrigPin GPIO7
#define EchoPin GPIO0

bool detect_obj(int cm, int repeat);
int detect_distance();
void init_ultra();
