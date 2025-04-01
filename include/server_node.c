#include"opc_ua/opc_ua_server.h"


void* updateThread(void *arg) {
    UA_Server *server = (UA_Server *)arg;

    while (1) {
        // updateVariable(server);
        sleep(10);  
    }
    return NULL;
}


static void serverRun() {
    UA_Server *server = UA_Server_new();
    UA_ServerConfig *config = UA_Server_getConfig(server);
    
    UA_String customServerUrls[1];
    customServerUrls[0] = UA_STRING("opc.tcp://192.168.192.185:4840");
    size_t customServerUrlsSize = 1;

    config->serverUrls = customServerUrls;
    config->serverUrlsSize = customServerUrlsSize;
    config->applicationDescription.applicationName = UA_LOCALIZEDTEXT_ALLOC("en", "Example for Medium");

    add_object_node(server, "Stacja pogodowa", 1001, UA_NS0ID(OBJECTSFOLDER));
    add_object_node(server, "Dane pogodowe", 2001, UA_NODEID_NUMERIC(1, 1001));
    add_string_node(server, "Data", 2101, 2001);
    add_float_array_node(server, "Temperatury", 2201, 2001);
    add_object_node(server, "Stacja pogodowa", 3001, UA_NODEID_NUMERIC(1, 1001));
    add_object_node(server, "PSE", 4001, UA_NODEID_NUMERIC(1, 1001));

    pthread_t thread;
    pthread_create(&thread, NULL, updateThread, (void *)server);
    printf("OPC UA Server started...\n");
    UA_Server_runUntilInterrupt(server);
    UA_Server_delete(server);
}


int main(void) {
    serverRun();
    return 0;
}
