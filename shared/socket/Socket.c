#include "Socket.h"
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// pridané kvôli funkcii getaddrinfo a vecí okolo nej
#ifndef __USE_XOPEN2K
#define __USE_XOPEN2K
#include <netdb.h>
#undef __USE_XOPEN2K
#else
#include <netdb.h>
#endif

// Funkcia na inicializáciu soketu, pričom je potrebné uviesť komunikačnú doménu, typ komunikácie a protokol
void socket_init(socket_data_t *this, int domain, int type, int protocol) {
  this->socket = socket(domain, type, protocol);
  if (this->socket < 0) {
    perror("socket_init: zlyhanie vytvorenia soketu!");
    exit(EXIT_FAILURE);
  }
}

// Funkcia na zničenie soketu
void socket_destroy(socket_data_t *this) {
  close(this->socket);
  this->socket = -1;
}

// Funkcia, ktorá vráti informáciu o tom, či je soket validný, teda či jeho popisova má hodnotu väčšiu ako 0
_Bool socket_is_valid(socket_data_t *this) {
  return this->socket >= 0;
}

// Funkcia, ktorá obaľuje funkciu bind, pričom je potrebné uviesť aj adresu servera a veľkosť adresy servera
void socket_bind(socket_data_t *this, const struct sockaddr *serverAddress, socklen_t serverAddressLength) {
  if (bind(this->socket, serverAddress, serverAddressLength) < 0) {
    perror("socket_bind: zlyhanie funkcie bind!");
    exit(EXIT_FAILURE);
  }
}

// Funkcia, ktorá obaľuje funkciu listen, pričom je potrebné uviesť aj hodnotu maximálneho počtu čakajúcich pripojení
void socket_listen(socket_data_t *this, int backlog) {
  if (listen(this->socket, backlog) < 0) {
    perror("socket_listen: zlyhanie funkcie listen!");
    exit(EXIT_FAILURE);
  }
}

// Funkcia, ktorá obaľuje funkciu accept, pričom je potrebné uviesť aj pasívny soket, miesto, kam sa uloží adresa klienta a miesto, kam sa uloží veľkosť adresy klienta
void socket_accept(socket_data_t *this, const socket_data_t *passiveSocket, struct sockaddr *clientAddress, socklen_t *clientAddressLength) {
  this->socket = accept(passiveSocket->socket, clientAddress, clientAddressLength);
  if (this->socket < 0) {
    perror("socket_accept: zlyhanie funkcie accept!");
    exit(EXIT_FAILURE);
  }
}

// Funkcia, ktorá obaľuje funkciu connect, pričom je potrebné uviesť aj adresu servera a veľkosť adresy servera
_Bool socket_connect(socket_data_t *this, const struct sockaddr *serverAddress, socklen_t serverAddressLength) {
  int result = connect(this->socket, serverAddress, serverAddressLength);
  printf("%d\n", result);
  if (result < 0) {
    perror("socket_connect: zlyhanie funkcie connect!");
  }
  return result == 0;
}

// Funkcia, ktorá obaľuje funkciu write, pričom je potrebné uviesť aj miesto, kde sa nachádzajú údaje a veľkosť miesta, z ktorého sa číta
void socket_write(socket_data_t *this, const char *buffer, size_t length) {
  int n = write(this->socket, buffer, length);
  if (n < 0) {
    perror("socket_write: zlyhanie zapisu do soketu!");
    exit(EXIT_FAILURE);
  }
}

// Funkcia, ktorá obaľuje funkciu read, pričom je potrebné uviesť aj miesto, kde sa uloží výsledok a maximálnu veľkosť miesta na uloženie
void socket_read(socket_data_t *this, char *buffer, size_t length) {
  int n = read(this->socket, buffer, length);
  if (n < 0) {
    perror("socket_read: zlyhanie citania z soketu!");
    exit(EXIT_FAILURE);
  }
}

// Funkcia na inicializáciu servera, pričom je potrebné uviesť aj port, na ktorom bude server čakať na pripojenia
void socket_server_init(socket_server_t *this, int port) {
  // Adresa servera
  struct sockaddr_in severAddress;
  // Vytvorenie pasívneho soketu pre TCP/IPv4 komunikáciu
  socket_init(&this->passiveSocket, AF_INET, SOCK_STREAM, IPPROTO_TCP);
  // Nastavenie adresy soketu, pričom sa jedná o internetovú komunikáciu
  memset(&severAddress, '\0', sizeof(severAddress));
  // IPv4
  severAddress.sin_family = AF_INET;
  // Použije sa localhost, teda adresa 127.0.0.1
  severAddress.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  // Nastaví sa port soketu podľa parametru port, pričom je potrebné brať do úvahy endianitu (htons)
  severAddress.sin_port = htons(port);
  // Samotné zavolanie funkcie bind, pričom sa pretypuje internetová adresa servera na generický typ adresy
  socket_bind(&this->passiveSocket, (const struct sockaddr *) &severAddress,
  sizeof(severAddress));
  // Premena soketu na pasívny socket, na ktorom sa bude prijímať spojenie, pričom sa budú ukladať 1 ďalšie čakajúce pripojenie do frontu
  socket_listen(&this->passiveSocket, 1);
  this->port = port;
  printf("[SERVER] bind+listen OK\n");
  fflush(stdout);
}

// Funkcia na akceptovanie pripojenia klientom, pričom sa jedná o blokovacie volanie
void socket_server_accept_connection(socket_server_t *this) {
  // Adresa klienta
  struct sockaddr_in clientAddress;
  // Ukladanie veľkosti adresy klienta
  socklen_t clientAddressSize = sizeof(clientAddress);

  // Pripojenie klienta na server, pričom sa nastaví všetko potrebné v adrese klienta, nastaví sa veľkosť adresy a vráti sa popisovač soketu určeného pre komunikáciu
  socket_accept(&this->activeSocket, &this->passiveSocket, (struct sockaddr *) &clientAddress, &clientAddressSize);
}

// Funkcia na zničenie servera, čo momentálne znamená zničenie pasívneho a aktívneho soketu
void socket_server_destroy(socket_server_t *this) {
  // Zničenie pasívneho soketu na prijímanie pripojení
  socket_destroy(&this->passiveSocket);
  // Zničenie aktívneho soketu pre komunikáciu s klientom
  socket_destroy(&this->activeSocket);
}

// Funkcia na inicializáciu klienta, pričom je potrebné uviesť aj názov servera a port, na ktorom bude zadaný server čakať na pripojenia
_Bool socket_client_init(socket_client_t *this, char *serverName, char *port) {
  // Štruktúra používaná pre potreby uloženia adries servera
  struct addrinfo *server;
  // Štruktúra používaná na definovanie základných informácií o type komunikácie
  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  // Internetová komunikácie použitím IP4 aj IP6
  hints.ai_family = AF_INET;

  // Spoľahlivá forma komunikácie
  hints.ai_socktype = SOCK_STREAM;
  // Upresnenie pre protokol TCP/IP
  hints.ai_protocol = IPPROTO_TCP;
  // Aj IP4 aj IP6 adresy
  hints.ai_flags = AI_ALL;
  // Zisťovanie adresy servera podľa mena, portu a uvedených informácií o type komunikácie
  int s = getaddrinfo(serverName, port, &hints, &server);
  if (s != 0) {
    fprintf(stderr, "socket_client_init: zlyhanie funkcie getaddrinfo(%d): %s\n", s, gai_strerror(s));
    exit(EXIT_FAILURE);
  }
  // Pozor! V určitých prípadoch môže existovať aj viac dostupných adries
  for (struct addrinfo *rp = server; rp != NULL; rp = rp->ai_next) {
    // Vytvorenie soketu pre komunikáciu cez internet, pričom sa používa získaná informácia
    socket_init(&this->activeSocket, rp->ai_family, rp->ai_socktype, rp->ai_protocol);
    // Ak nedošlo k vytvoreniu soketu
    if (!socket_is_valid(&this->activeSocket)) {
    // Pokračuj ďalšou adresou
      continue;
    }
    // Pripojenie na server
    printf("Klient sa pripaja na server!\n");
    if (!socket_connect(&this->activeSocket, rp->ai_addr, rp->ai_addrlen)) {
      // Ak zlyhalo, tak sa zatvorí soket a pokračuje sa ďalšou adresou
      printf("Zlyhanie pripojenia!\n");
      socket_destroy(&this->activeSocket);
      return 0;
    } else {
      // Ak sa podarilo pripojiť, uvoľní sa získaná štruktúra z funkcie getaddrinfo a nastaví sa pomocná premenná na úspešné pripojenie
      freeaddrinfo(server);
      this->serverName = serverName;
      this->port = atoi(port);
      return 1;
    }
  }

  // Pokiaľ po prejdení všetkých adries nedošlo k pripojeniu, tak vypíš chybu a skonči
  fprintf(stderr, "socket_client_init: zlyhalo pripojenie na server!\n");
  freeaddrinfo(server);
  exit(EXIT_FAILURE);
}
// Funkcia na zničenie klienta, čo znamená zničenie aktívneho soketu
void socket_client_destroy(socket_client_t *this) {
  // Zničenie aktívneho soketu pre komunikáciu so serverom
  socket_destroy(&this->activeSocket);
}


