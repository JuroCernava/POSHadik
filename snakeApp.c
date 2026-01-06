#include "./client/Client.h"
#include <stdio.h>
int main() {
  client_t client;
  client_init(&client);
  client_listen(&client);
  return 0;
}
