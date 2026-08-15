#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <stdint.h>

// CONSTANTS
#define BUFF_SIZE 4096

int setup_listening_socket(uint16_t port) {
  // socket ===================================
  // man 2 socket
  int sck = socket(AF_INET, SOCK_STREAM, 0); // This is a listenting socket open to receive reqeusts but it's different from the socket established with a client
  if (sck < 0) {
    perror("socket");
    return -1;
  }

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr)); // Initialize this memory segment to 0s (it isn't valid for all data types for this case it's ok)
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port); // little-endian -> big-endian | h: host, to, n: network, s: short
  addr.sin_addr.s_addr = htonl(INADDR_ANY); // h: host, to, n: network, l: long

  int opt = 1;
  int reuse_addr_r = setsockopt(sck, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
  if (reuse_addr_r < 0) {
    perror("setsockopt");
    return -1;
  }

  // bind ===================================
  // man 2 bind
  int bind_r = bind(sck, (struct sockaddr *) &addr, sizeof(addr)); // Casting in C works as: (type)value
  if (bind_r < 0) {
    perror("bind");
    return -1;
  }

  // listen ===================================
  // man 2 listen
  int listen_r = listen(sck, 16);
  if (listen_r < 0) {
    perror("listen");
    return -1;
  }

  return sck;
}

void handle_client(int client_fd) {
    // process the request
    char buff[BUFF_SIZE];
    memset(buff, 0, sizeof(buff));

    // read (2) returns the bytes read, and since this is a zero indexed operation this buffer will always write up to sizeof(buff) - 1
    // if we were BUFF_SIZE was 4096 we would be passing 4095 and read will return at maximum 4095 so it's okay if we do buff[bytes] = '\0'
    // for the null character to wrtie to stdout
    ssize_t bytes = read(client_fd, buff, sizeof(buff) - 1);
    //read validation
    if (bytes < 0) {
      perror("read");
      // close the connection without killing the server by freeing the socket first
      close(client_fd);
      return;
    } else if (bytes == 0) {
      printf("Connection closed...\n");
      close(client_fd);
      return;
    }

    buff[bytes] = '\0';

    //print the raw request

    printf("%s\n", buff);

    // build the resopnse
    char response[4096];
    memset(response, 0, sizeof(response));

    char body[1024];
    memset(body, 0, sizeof(body));

    // formatting the response

    int characs = snprintf(body, sizeof(body), "Hello World!");
    if (characs < 0) {
      perror("snprintf");
      close(client_fd);
      return;
    } else if (characs >= (int)sizeof(body)) {
      fprintf(stderr, "text is bigger than the body buffer");
      close(client_fd);
      return;
    }

    characs = snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: %d\r\n\r\n%s", characs, body);
    printf("sending:\n%s", response);
    if (characs < 0) {
      perror("snprintf");
      close(client_fd);
      return;
    } else if (characs >= (int)sizeof(response)) {
      fprintf(stderr, "text is bigger than the body buffer");
      close(client_fd);
      return;
    }

    // syscall writing the bytes in response to the client
    ssize_t char_r = write(client_fd, response, characs);
    if (char_r < 0) {
      perror("write");
      close(client_fd);
      return;
    }

    // close the communication socket
    close(client_fd);
    return;
}

int main () {
  // listen for requests at port 8080
  int sck = setup_listening_socket(8080);
  if (sck < 0) {
    return EXIT_FAILURE;
  }

  // accept requests
  while(1) {
    struct sockaddr_in client_address;
    socklen_t client_address_size = sizeof(client_address);
    memset(&client_address, 0, sizeof(client_address));

    // accept and open the communication socket with a specific client
    int client_fd = accept(sck, (struct sockaddr *) &client_address, &client_address_size);
    if (client_fd < 0) {
      perror("accept");
      return EXIT_FAILURE;
    }

    handle_client(client_fd);
  }

  // close the listening socket
  close(sck);
}
