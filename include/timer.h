
#ifndef VSX_TIMER_H
#define VSX_TIMER_H

//#ifndef WIN32
#include <time.h>
#include <sys/time.h>
//#endif

typedef struct vsx_timer {
  double startt;
  double lastt;
  double dtimet;
  LONGLONG init_time;
} vsx_timer;

typedef vsx_timer* Timer;

#ifdef _WIN32  
  LONGLONG init_time;
#endif

Timer CreateTimer()
{
	Timer t;
	t = (Timer)malloc(sizeof(vsx_timer));
#ifdef _WIN32
	LARGE_INTEGER time;
    QueryPerformanceCounter(&time);
	t->init_time = time.QuadPart;
#endif	
	t->startt = 0;
	t->lastt = 0;
	t->dtimet = 0;
	return(t);
}
  
void DestroyTimer(Timer t)
{
	free(t);
}

// accurate time
double TimerATime(Timer t) {

#ifdef _WIN32
  	LARGE_INTEGER freq, time;
  	QueryPerformanceFrequency(&freq);
  	QueryPerformanceCounter(&time);  
	
	double ret = (double)((double)(time.QuadPart - (double)t->init_time) / (double)freq.QuadPart);
	//double ret = (double)(time.QuadPart - t->init_time);
	//printf("%llu<br/>",(double)time.QuadPart);
	//printf("%f<br/>",((double)time.QuadPart)/((double)freq.QuadPart));
	
	
    return  ret;
#else
    struct timeval now;
    gettimeofday(&now, 0);
    return (double)now.tv_sec+0.000001*(double)now.tv_usec;
#endif
}  
  
void StartTimer(Timer t) {
	t->startt = TimerATime(t);
    t->lastt = t->startt;
}

double TimerDiffTime(Timer t) {
    double at = TimerATime(t);
    t->dtimet = at - t->lastt;
    t->lastt = at;
    return t->dtimet;
}

// normal time
double ntime() {
  return ((double)clock())/((double)CLOCKS_PER_SEC);
}

#endif