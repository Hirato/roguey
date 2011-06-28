#include "engine.h"

//mersenne's twister sourced from cube2

#define N (624)
#define M (397)
#define K (0x9908B0DFU)

static unsigned int state[N];
static int next = N;

void seedMT(unsigned int seed)
{
    state[0] = seed;
    for(unsigned int i = 1; i < N; i++)
        state[i] = seed = 1812433253U * (seed ^ (seed >> 30)) + i;
    next = 0;
}

unsigned int randomMT()
{
    int cur = next;
    if(++next >= N)
    {
        if(next > N) { seedMT(5489U + time(NULL)); cur = next++; }
        else next = 0;
    }
    unsigned int y = (state[cur] & 0x80000000U) | (state[next] & 0x7FFFFFFFU);
    state[cur] = y = state[cur < N-M ? cur + M : cur + M-N] ^ (y >> 1) ^ (-(y & 1U) & K);
    y ^= (y >> 11);
    y ^= (y <<  7) & 0x9D2C5680U;
    y ^= (y << 15) & 0xEFC60000U;
    y ^= (y >> 18);
    return y;
}
