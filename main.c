#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>

int main () {
  // socket
  int sck = socket(AF_INET, SOCK_STREAM, 0);
  if (sck < 0) {
    perror("socket");
    return EXIT_FAILURE;
  }

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(8080);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);

  // bind
  int bind_r = bind(sck, (struct sockaddr *) &addr, sizeof(addr));

  if (bind_r < 0) {
    perror("bind");
    return EXIT_FAILURE;
  }

  // listen
  // accept
}
