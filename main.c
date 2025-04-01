#include "include/client_node.h"

pthread_t tid[3];
float data [4][73];
float averages[4];

void* calc_mean(){
    float averages[4];
    for (int row = 0; row < 4; row++) {
            float sum = 0.0;
            for (int col = 0; col < 73; col++) {
                sum += data[row][col];
            }
            averages[row] = sum / 73.0;  // Oblicz średnią dla wiersza
        }
}


void* calc_thread(void *arg) {
    while (1) {
        if (time_idx==DATA_ARRAY_SIZE &&  temperature_idx==DATA_ARRAY_SIZE && 
                wind_speed_idx==DATA_ARRAY_SIZE && cloudiness_idx==DATA_ARRAY_SIZE){
            pthread_mutex_lock(&mutex);
            pthread_mutex_lock(&mutex2);

            printf("calcThread\n");
            // calc_mean();

            time_idx = 0;
            temperature_idx = 0;
            wind_speed_idx = 0;
            cloudiness_idx = 0;

            pthread_mutex_unlock(&mutex2);
            pthread_mutex_unlock(&mutex);
            sleep(90 * 60);
        }
        // printf("skipped %d, %d, %d, %d\n", time_idx, temperature_idx, wind_speed_idx, cloudiness_idx);
        sleep(6);
    }
    return NULL;
}

void* client_thread(void *arg){
    printf("OPC UA Client started...\n");
    clientRun();
}

void* send_thread(void *arg){
    // while (1) {
    //     pthread_mutex_lock(&lock[1]);
    //     // TODO: send data
    //     printf("sendThread\n");
    //     sleep(1);
    //     pthread_mutex_unlock(&lock[1]);
    //     sleep(1);
    // }
    return NULL;
}

int main(){
    if (pthread_mutex_init(&mutex, NULL) != 0 && pthread_mutex_init(&mutex2, NULL)){
        printf("ERROR initializing mutex\n");
        /* ERROR */
    }
    
    pthread_create(&(tid[0]) ,NULL, calc_thread, NULL);
    pthread_create(&(tid[1]) ,NULL, client_thread, NULL);
    pthread_create(&(tid[2]) ,NULL, send_thread, NULL);

    while (1) {}
}