#include "client_node.c"


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


void* calcThread(void *arg) {
    while (1) {
        pthread_mutex_lock(&lock[0]);
        pthread_mutex_lock(&lock[1]);
        calc_mean();
        printf("test\n");
        pthread_mutex_unlock(&lock[1]);
        pthread_mutex_unlock(&lock[0]);
    }
    return NULL;
}

void* clientThread(void *arg){
    printf("OPC UA Client started...\n");
    clientRun();
}

void* sendThread(void *arg){
    
}

int main(){
    int pthread_mutex_init(pthread_mutex_t *restrict mutex,
        const pthread_mutexattr_t *restrict attr);
    
    pthread_create(&(tid[0]) ,NULL, calcThread, NULL);
    pthread_create(&(tid[1]) ,NULL, clientThread, NULL);
    pthread_create(&(tid[2]) ,NULL, sendThread, NULL);
}