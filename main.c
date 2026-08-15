#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>

int main () {
  // socket ===================================
  // man 2 socket
  int sck = socket(AF_INET, SOCK_STREAM, 0);
  if (sck < 0) {
    perror("socket");
    return EXIT_FAILURE;
  }

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr)); // inicia este segmento de memoria en 0s (no es lo mismo para todos los tipos de datos)
  addr.sin_family = AF_INET;
  addr.sin_port = htons(8080); // little-endian -> big-endian | h: host, to, n: network, s: short
  addr.sin_addr.s_addr = htonl(INADDR_ANY); // h: host, to, n: network, l: long

  // bind ===================================
  // man 2 bind
  int bind_r = bind(sck, (struct sockaddr *) &addr, sizeof(addr)); // El casteo en C funcina como (tipo)valor
  if (bind_r < 0) {
    perror("bind");
    return EXIT_FAILURE;
  }

  // listen ===================================
  // man 2 listen
  int listen_r = listen(sck, 16);
  if (listen_r < 0) {
    perror("listen");
    return EXIT_FAILURE;
  }
  // accept ===================================
}
