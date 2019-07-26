#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#include <MQTTAsync.h>

#define BROKER_URL "tcp://0.0.0.0:1883"
#define CLIENT_ID "v1/service-agents/alpha.mqtt5-test.svc.example.org"
#define KEEP_ALIVE_INTERVAL 60
#define SUBSCRIBE_TOPIC "agents/alpha.mqtt5-test.svc.example.org/api/v1/in/client.svc.example.org"
#define SUBSCRIBE_QOS 1
#define DISCONNECT_TIMEOUT 1000

typedef struct Context {
  MQTTAsync client;
  bool halt;
} Context;

void on_connection_lost(void *ctx, char *cause);
int on_message_arrived(void *ctx, char *topic, int topic_len, MQTTAsync_message *message);
void on_delivery_complete(void *ctx, MQTTAsync_token token);
void on_connect_success(void *ctx, MQTTAsync_successData5 *response);
void on_connect_failure(void *ctx, MQTTAsync_failureData5 *response);
void on_disconnect_success(void *ctx, MQTTAsync_successData5 *response);
void on_disconnect_failure(void *ctx, MQTTAsync_failureData5 *response);
void on_subscribe_success(void *ctx, MQTTAsync_successData5 *response);
void on_subscribe_failure(void *ctx, MQTTAsync_failureData5 *response);
