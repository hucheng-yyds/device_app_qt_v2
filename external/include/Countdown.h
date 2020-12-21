#ifndef COUNTDOWN_H
#define COUNTDOWN_H

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <sys/time.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include <stdlib.h>
#include <string.h>
#include <signal.h>

class Countdown
{
public:
    Countdown()
    {

    }

    Countdown(int ms)
    {
        countdown_ms(ms);
    }


    bool expired()
    {
        struct timeval now, res;
        gettimeofday(&now, NULL);
        timersub(&end_time, &now, &res);
        //printf("left %d ms\n", (res.tv_sec < 0) ? 0 : res.tv_sec * 1000 + res.tv_usec / 1000);
        //if (res.tv_sec > 0 || res.tv_usec > 0)
        //	printf("expired %d %d\n", res.tv_sec, res.tv_usec);
        return res.tv_sec < 0 || (res.tv_sec == 0 && res.tv_usec <= 0);
    }


    void countdown_ms(int ms)
    {
        end_ms = ms;
        struct timeval now;
        gettimeofday(&now, NULL);
        struct timeval interval = {ms / 1000, (ms % 1000) * 1000};
        //printf("interval %d %d\n", interval.tv_sec, interval.tv_usec);
        timeradd(&now, &interval, &end_time);
    }


    void countdown(int seconds)
    {
        struct timeval now;
        gettimeofday(&now, NULL);
        struct timeval interval = {seconds, 0};
        timeradd(&now, &interval, &end_time);
    }


    int left_ms()
    {
        struct timeval now, res;
        gettimeofday(&now, NULL);
        timersub(&end_time, &now, &res);
        //printf("left %d ms\n", (res.tv_sec < 0) ? 0 : res.tv_sec * 1000 + res.tv_usec / 1000);
        return (res.tv_sec < 0) ? 0 : res.tv_sec * 1000 + res.tv_usec / 1000;
    }

    int right_ms()
    {
        return end_ms - left_ms();
    }

private:

    struct timeval end_time;
    int end_ms;
};

#endif // COUNTDOWN_H
