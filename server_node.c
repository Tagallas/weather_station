#include <open62541/server.h>
#include <open62541/server_config_default.h>
#include <open62541/plugin/log_stdout.h>
#include <open62541/types_generated.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

typedef struct {
    int intValue;
    char stringValue[100];
} CustomData;

static void updateVariable(UA_Server *server) {
    static int counter = 0;
    CustomData data;
    data.intValue = counter;
    snprintf(data.stringValue, sizeof(data.stringValue), "Message %d", counter);
    counter++;

    // Tworzenie wariantu OPC UA
    UA_Variant value;
    UA_Variant_init(&value);
    UA_String uaString = UA_STRING(data.stringValue);
    UA_Variant_setScalar(&value, &uaString, &UA_TYPES[UA_TYPES_STRING]);

    // Aktualizacja wartości węzła
    UA_NodeId nodeId = UA_NODEID_STRING(1, "customVariable");
    UA_Server_writeValue(server, nodeId, value);
}

static void addVariable(UA_Server *server) {
    UA_VariableAttributes attr = UA_VariableAttributes_default;
    attr.displayName = UA_LOCALIZEDTEXT("en-US", "Custom Variable");
    attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

    UA_String initialString = UA_STRING("Initial Value");
    UA_Variant_setScalar(&attr.value, &initialString, &UA_TYPES[UA_TYPES_STRING]);

    UA_NodeId nodeId = UA_NODEID_STRING(1, "customVariable");
    UA_QualifiedName name = UA_QUALIFIEDNAME(1, "customVariable");
    UA_NodeId parentNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    UA_Server_addVariableNode(server, nodeId, parentNodeId, UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT), name,
                              UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), attr, NULL, NULL);
}

void* updateThread(void *arg) {
    UA_Server *server = (UA_Server *)arg;

    while (1) {
        // Co sekundę aktualizujemy zmienną
        updateVariable(server);
        sleep(1);  // Pauza na 1 sekundę
    }
    return NULL;
}

// static void serverRun() {
//     UA_Server *server = UA_Server_new();
//     UA_ServerConfig_setDefault(UA_Server_getConfig(server));

//     addVariable(server);

//     //while (true) {
//     //    updateVariable(server);
//     //    UA_Server_run_iterate(server, true);
//         //UA_sleep_ms(1000);  // Co sekundę aktualizuje zmienną
// 	//sleep(1);
//     //}
//     UA_Server_runUntilInterrupt(server);
//     UA_Server_delete(server);
// }

int main(void) {
    UA_Server *server = UA_Server_new();
    UA_ServerConfig_setDefault(UA_Server_getConfig(server));

    // zmiana adresu działania servera
    // UA_ServerConfig *config = UA_Server_getConfig(server);
    // config->networkLayersSize = 1;
    // config->networkLayers = (UA_ServerNetworkLayer *)UA_malloc(sizeof(UA_ServerNetworkLayer));
    // *config->networkLayers = UA_ServerNetworkLayerTCP(UA_ConnectionConfig_default, 4840, "opc.tcp://192.168.1.10:4840");

    addVariable(server);
    
    pthread_t thread;
    pthread_create(&thread, NULL, updateThread, (void *)server);
    printf("OPC UA Server started...\n");
    UA_Server_runUntilInterrupt(server);
    UA_Server_delete(server);
    return 0;
}