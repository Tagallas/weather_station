#include "include/client_node.h"

pthread_t tid[3];

#define DATE_FORMAT "%d/%m/%Y %H:%M"

int compare_dates(char* date1, char* date2) {
    struct tm tm1 = {0}, tm2 = {0};

    char str_date1[17];
    strncpy(str_date1, date1, sizeof(str_date1));
    str_date1[16] = '\0';  
    char str_date2[17];
    strncpy(str_date2, date2, sizeof(str_date2));
    str_date2[16] = '\0';

    strptime(str_date1, DATE_FORMAT, &tm1);
    strptime(str_date2, DATE_FORMAT, &tm2);

    time_t time1 = mktime(&tm1);
    time_t time2 = mktime(&tm2);

    if (time1 < time2) return -1;
    if (time1 > time2) return 1;
    return 0;
}


void* calc_mean(){
    for (int i = 0; i < DATA_ARRAY_SIZE; i++) {
        if (compare_dates((time_table[i]), max_time) > 0) {
            strncpy(max_time, time_table[i], sizeof(max_time));
        }
        temperature_sum += temperature_table[i];
        wind_speed_sum += wind_speed_table[i];
        cloudiness_sum += cloudiness_table[i];
    }

    temperature_sum = temperature_sum / DATA_ARRAY_SIZE; 
    wind_speed_sum = wind_speed_sum / DATA_ARRAY_SIZE; 
    cloudiness_sum = cloudiness_sum / DATA_ARRAY_SIZE; 
}


void* calc_thread(void *arg) {
    while (1) {
        // printf("lock calcThread\n");
        pthread_mutex_lock(&mutex2);

        while (time_idx<DATA_ARRAY_SIZE) {
            sleep(6); 
        }

        pthread_mutex_lock(&mutex);

        printf("calcThread\n");
        // printf("idx: time: %d, temp: %d, wind: %d, cloud: %d\n", time_idx, temperature_idx, wind_speed_idx, cloudiness_idx);
        calc_mean();
        printf("time: %s, temp: %.2f, wind: %.2f, cloud: %d\n", max_time, temperature_sum, wind_speed_sum, cloudiness_sum);
        time_idx = 0;
        temperature_idx = 0;
        wind_speed_idx = 0;
        cloudiness_idx = 0;

        pthread_mutex_unlock(&mutex);
        pthread_mutex_unlock(&mutex2);

        sleep(6);
        // printf("skipped %d, %d, %d, %d\n", time_idx, temperature_idx, wind_speed_idx, cloudiness_idx);
    }
    return NULL;
}

void* client_thread(void *arg){
    UA_Client *client = (UA_Client *)arg;

    printf("OPC UA Client started...\n");
    client_run(client);
}

void* send_thread(void *arg){
    while (1) {
        pthread_mutex_lock(&mutex2);
        
        printf("sendThread\n");
        send_data(arg);

        temperature_sum = 0;
        wind_speed_sum = 0;
        cloudiness_sum = 0;
        // printf("cleared\n");
        // printf("idx: time: %d, temp: %d, wind: %d, cloud: %d\n", time_idx, temperature_idx, wind_speed_idx, cloudiness_idx);
        // printf("time: %s, temp: %.2f, wind: %.2f, cloud: %d\n", max_time, temperature_sum, wind_speed_sum, cloudiness_sum);
        
        pthread_mutex_unlock(&mutex2);
        sleep(6);
    }
    return NULL;
}

int main(){
    if (pthread_mutex_init(&mutex, NULL) != 0 && pthread_mutex_init(&mutex2, NULL)){
        printf("ERROR initializing mutex\n");
        /* ERROR */
    }

    pthread_create(&(tid[0]) ,NULL, calc_thread, NULL);
    pthread_create(&(tid[1]) ,NULL, client_thread, NULL);
    sleep(1);
    pthread_create(&(tid[2]) ,NULL, send_thread, NULL);

    while (1) {}
}

