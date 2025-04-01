#ifndef GLOBAL_H
#define GLOBAL_H

#include <open62541/client.h>
#include <pthread.h>

#define DATA_ARRAY_SIZE 69


typedef struct {
    char cities[100];
    double temperature;
    double wind_speed;
    int cloudiness;
} WeatherData;

typedef struct {
    UA_String time;
    double temperature;
    double wind_speed;
    int cloudiness;
} ReceivedWeatherData;



#endif  