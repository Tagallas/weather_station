#include "client_node.h"

pthread_mutex_t mutex;
pthread_mutex_t mutex2;

UA_String time_table[DATA_ARRAY_SIZE];
double temperature_table[DATA_ARRAY_SIZE];
double wind_speed_table[DATA_ARRAY_SIZE];
int cloudiness[DATA_ARRAY_SIZE];

volatile int time_idx = 0;
volatile int temperature_idx = 0;
volatile int wind_speed_idx = 0;
volatile int cloudiness_idx = 0;

static void dataChangeCallbackTime(UA_Client *client, UA_UInt32 subId, void *subContext,
    UA_UInt32 monId, void *monContext, UA_DataValue *value) {
    if (UA_Variant_hasScalarType(&value->value, &UA_TYPES[UA_TYPES_STRING])) {

        UA_String *str = (UA_String *)value->value.data;
        printf("Received update from time: %.*s\n", (int)str->length, str->data);

        pthread_mutex_lock(&mutex);
        
        time_table[time_idx] = *str;
        ++time_idx;

        pthread_mutex_unlock(&mutex);
    }
}

static void dataChangeCallbackTemp(UA_Client *client, UA_UInt32 subId, void *subContext,
    UA_UInt32 monId, void *monContext, UA_DataValue *value) {
    if (UA_Variant_hasScalarType(&value->value, &UA_TYPES[UA_TYPES_DOUBLE])) {

        UA_Double data = *(UA_Double *)value->value.data;
        printf("Received update from temperature: %.2f\n", data);

        pthread_mutex_lock(&mutex);
        
        temperature_table[temperature_idx] = (double)data;
        ++temperature_idx;

        pthread_mutex_unlock(&mutex);
    }
}

static void dataChangeCallbackWind(UA_Client *client, UA_UInt32 subId, void *subContext,
    UA_UInt32 monId, void *monContext, UA_DataValue *value) {
    if (UA_Variant_hasScalarType(&value->value, &UA_TYPES[UA_TYPES_DOUBLE])) {

        UA_Double data = *(UA_Double *)value->value.data;
        printf("Received update from wind speed: %.2f\n", data);

        pthread_mutex_lock(&mutex);
        
        wind_speed_table[wind_speed_idx] = (double)data;
        ++wind_speed_idx;

        pthread_mutex_unlock(&mutex);
    }
}

static void dataChangeCallbackCloud(UA_Client *client, UA_UInt32 subId, void *subContext,
    UA_UInt32 monId, void *monContext, UA_DataValue *value) {
    if (UA_Variant_hasScalarType(&value->value, &UA_TYPES[UA_TYPES_INT32])) {

        UA_Int32 data = *(UA_Int32 *)value->value.data;
        printf("Received update from cloudness: %d\n", data);

        pthread_mutex_lock(&mutex);
        
        cloudiness[cloudiness_idx] = (int)data;
        ++cloudiness_idx;

        pthread_mutex_unlock(&mutex);
    }
}

void clientRun() {
    UA_Client* client = create_and_start_opc_ua_client("opc.tcp://192.168.192.185:4840");

    for (int i = 0; i < DATA_ARRAY_SIZE; i++) {
        add_subscription(client, nodeIDs[i]+1, dataChangeCallbackTime);
        add_subscription(client, nodeIDs[i]+2, dataChangeCallbackTemp);
        add_subscription(client, nodeIDs[i]+3, dataChangeCallbackWind);
        add_subscription(client, nodeIDs[i]+4, dataChangeCallbackCloud);
    }

    while (true) {
        UA_Client_run_iterate(client, 10 * 1000);

        // dataChangeCallback(client, 2105, NULL, 0, NULL, NULL);
        // printf("%.2f\n", wind_speed_table[10]);
        // sleep(5);
    }

    UA_Client_delete(client);
}

//  int main(void) {
//      printf("OPC UA Client started...\n");
//      clientRun();
//      return 0;
// }
