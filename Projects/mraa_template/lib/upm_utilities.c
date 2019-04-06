#include <assert.h>
#include <time.h>
#include <errno.h>
#include "upm_utilities.h"

/**
 * Calculate the delta of two upm_clock_t values as
 *      delta = finish - start
 *
 * @param finish Ending upm_clock_t time
 * @param start Beginning upm_clock_t time
 * @return Time in nanoseconds
 */
static uint64_t _delta_ns(const upm_clock_t* finish, const upm_clock_t* start)
{
    uint64_t delta;
    assert((finish != NULL) && (start != NULL) && "_delta_ns, arguments cannot be NULL");
    delta = (finish->tv_sec * 1000000000UL + finish->tv_nsec) -
        (start->tv_sec * 1000000000UL + start->tv_nsec);
    return delta;
}

// void upm_delay(uint32_t time)
// {
// 	if(!time)
// 		return;
// 	upm_clock_t delay_time = {time, 0};
// 	while(clock_nanosleep(CLOCK_MONOTONIC, 0, &delay_time, &delay_time) == EINTR);
// }

void upm_delay(uint32_t time)
{
	if(!time)
		return;
	upm_clock_t delay_time = {0, time};
	while(nanosleep(&delay_time, NULL));
}

void upm_delay_ms(uint32_t time)
{
	if(!time)
		return;
	while(usleep(time*1000));
}

void upm_delay_us(uint32_t time)
{
	if(!time)
		return;
	while(usleep(time));
}

void upm_delay_ns(uint64_t time)
{
	if(!time)
		return;
	upm_clock_t delay_time = {0, time};
	while(nanosleep(&delay_time, NULL));
}

upm_clock_t upm_clock_init(void)
{
	upm_clock_t clock = {0};
	clock_gettime(CLOCK_MONOTONIC, &clock);
	return clock;
}

uint64_t upm_elapsed_ms(const upm_clock_t *clock)
{
	assert((clock != NULL) && "upm_elapsed_ms, clock cannot be NULL");
	upm_clock_t now = {0};
	clock_gettime(CLOCK_MONOTONIC, &now);
	return _delta_ns(&now, clock)/1000000;
}

uint64_t upm_elapsed_us(const upm_clock_t *clock)
{
	assert((clock != NULL) && "upm_elapsed_us, clock cannot be NULL");
	upm_clock_t now = {0};
	clock_gettime(CLOCK_MONOTONIC, &now);
	return _delta_ns(&now, clock)/1000;
}

uint64_t upm_elapsed_ns(const upm_clock_t *clock)
{
	assert((clock != NULL) && "upm_elapsed_ns, clock cannot be NULL");
	upm_clock_t now = {0};
	clock_gettime(CLOCK_MONOTONIC, &now);
	return _delta_ns(&now, clock);
}

// https://www3.epa.gov/airnow/aqi-technical-assistance-document-may2016.pdf
static struct aqi {
    float clow;
    float chigh;
    int llow;
    int lhigh;
} aqi[] = {
  {0.0,    12.0,   0, 50},
  {12.1,   35.4,  51, 100},
  {35.5,   55.4, 101, 150},
  {55.5,  150.4, 151, 200},
  {150.5, 250.4, 201, 300},
  {250.5, 350.4, 301, 400},
  {350.5, 500.4, 401, 500},
};


int upm_ugm3_to_aqi (double ugm3)
{
	int i;
	for(i = 0; i < 7; i++)
	{
		if(ugm3 >= aqi[i].clow && ugm3 <= aqi[i].chigh)
		{
			// Ip = [(Ihi-Ilow)/(BPhi-BPlow)](cp-BPlow)+Ilow,
			return ((aqi[i].lhigh - aqi[i].chigh)/(aqi[i].chigh - aqi[i].clow)) * (ugm3-aqi[i].clow) + aqi[i].llow;
		}
	}
	return 0;
}