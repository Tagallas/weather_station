#include "client_node.h"

pthread_mutex_t mutex;
pthread_mutex_t mutex2;

char time_table[DATA_ARRAY_SIZE][17];
double temperature_table[DATA_ARRAY_SIZE];
double wind_speed_table[DATA_ARRAY_SIZE];
int cloudiness_table[DATA_ARRAY_SIZE];

volatile int time_idx = 0;
volatile int temperature_idx = 0;
volatile int wind_speed_idx = 0;
volatile int cloudiness_idx = 0;

char max_time[17] = "01/01/2000 00:00";
double temperature_sum = 0;
double wind_speed_sum = 0;
int cloudiness_sum = 0;


int find_idx(int node_id){
    int base_ID = 2101;
    int cp = (node_id-base_ID)/10;
    int l1 = cp%10;
    cp = cp/10;
    int l2 = cp%10;

    return l2+9*l1;
}


static void dataChangeCallbackTime(UA_Client *client, UA_UInt32 subId, void *subContext,
    UA_UInt32 monId, void *monContext, UA_DataValue *value) {
    if (UA_Variant_hasScalarType(&value->value, &UA_TYPES[UA_TYPES_STRING])) {

        UA_String *str = (UA_String *)value->value.data;
        
        int *node_id_ptr = (int *)monContext;
        // printf("node id: %d | Received update from time: %.*s\n", *node_id_ptr, (int)str->length, str->data);

        pthread_mutex_lock(&mutex);
        
        int idx = find_idx(*node_id_ptr);
        if (str->length < 17) { 
            memcpy(time_table[idx], str->data, str->length);
            time_table[idx][str->length] = '\0'; 
        } else {
            memcpy(time_table[idx], str->data, 16);
            time_table[idx][16] = '\0';
        }
        // printf("Value: %s\n", time_table[time_idx]);
        ++time_idx;

        pthread_mutex_unlock(&mutex);
    }
}

static void dataChangeCallbackTemp(UA_Client *client, UA_UInt32 subId, void *subContext,
    UA_UInt32 monId, void *monContext, UA_DataValue *value) {
    if (UA_Variant_hasScalarType(&value->value, &UA_TYPES[UA_TYPES_DOUBLE])) {

        UA_Double data = *(UA_Double *)value->value.data;

        int *node_id_ptr = (int *)monContext;
        // printf("node id: %d | Received update from temperature: %.2f\n", *node_id_ptr, data);

        pthread_mutex_lock(&mutex);
        
        int idx = find_idx(*node_id_ptr);
        temperature_table[idx] = (double)data;

        pthread_mutex_unlock(&mutex);
    }
}

static void dataChangeCallbackWind(UA_Client *client, UA_UInt32 subId, void *subContext,
    UA_UInt32 monId, void *monContext, UA_DataValue *value) {
    if (UA_Variant_hasScalarType(&value->value, &UA_TYPES[UA_TYPES_DOUBLE])) {

        UA_Double data = *(UA_Double *)value->value.data;

        int *node_id_ptr = (int *)monContext;
        // printf("node id: %d | Received update from wind speed: %.2f\n", *node_id_ptr, data);

        pthread_mutex_lock(&mutex);
        
        int idx = find_idx(*node_id_ptr);
        wind_speed_table[idx] = (double)data;

        pthread_mutex_unlock(&mutex);
    }
}

static void dataChangeCallbackCloud(UA_Client *client, UA_UInt32 subId, void *subContext,
    UA_UInt32 monId, void *monContext, UA_DataValue *value) {
    if (UA_Variant_hasScalarType(&value->value, &UA_TYPES[UA_TYPES_INT32])) {

        UA_Int32 data = *(UA_Int32 *)value->value.data;

        int *node_id_ptr = (int *)monContext;
        // printf("node id: %d | Received update from cloudness: %d\n", *node_id_ptr, data);

        pthread_mutex_lock(&mutex);
        
        int idx = find_idx(*node_id_ptr);
        cloudiness_table[idx] = (int)data;

        pthread_mutex_unlock(&mutex);
    }
}

void send_data(){
    UA_Client* client = create_and_start_opc_ua_client("opc.tcp://192.168.192.26:4840");
    
    write_to_string_node(client, 3002, max_time);
    write_to_double_node(client, 3003, temperature_sum);
    write_to_double_node(client, 3004, wind_speed_sum);
    write_to_int32_node(client, 3005, cloudiness_sum);
    
    UA_Client_delete(client);

    printf("Data Send\n");
}

void client_run() {
    UA_Client* client = create_and_start_opc_ua_client("opc.tcp://192.168.192.26:4840"); // wojtek: 185

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
//      client_run();
//      return 0;
// }
