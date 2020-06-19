#include <stdlib.h>
#include "ehlo-shared.h"

int main(int argc, char **argv)
{
  int error;
  socket_t server_sock;
  struct sockaddr_in server_addr;
  #ifdef _WIN32
    int wsa_error;
    WSADATA wsa_data;
  #endif
  const char *host, *port;

  if (argc < 3) {
    fprintf(stderr, "Usage: %s <host> <port>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  #ifdef _WIN32
    wsa_error = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if (wsa_error != 0) {
      fprintf(stderr, "WSAStartup: %s\n",
          get_error_string(wsa_error, NULL, 0));
      exit(EXIT_FAILURE);
    }
  #endif

  host = argv[1];
  port = argv[2];

  server_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (server_sock == -1) {
    fprintf(stderr, "socket: %s\n",
        get_error_string(get_socket_error(), NULL, 0));
    exit(EXIT_FAILURE);
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(atoi(port));

  error = bind(server_sock,
               (struct sockaddr *)&server_addr,
               sizeof(server_addr));
  if (error != 0) {
    fprintf(stderr, "bind: %s\n",
        get_error_string(get_socket_error(), NULL, 0));
    close_socket(server_sock);
    exit(EXIT_FAILURE);
  }

  error = listen(server_sock, 0);
  if (error != 0) {
    fprintf(stderr, "listen: %s\n",
        get_error_string(get_socket_error(), NULL, 0));
    close_socket(server_sock);
    exit(EXIT_FAILURE);
  }

  printf("Listening at %s:%s\n", host, port);

  for (;;) {
    socket_t client_sock;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    client_sock = accept(server_sock,
                         (struct sockaddr *)&client_addr,
                         &client_addr_len);
    if (client_sock == -1) {
      fprintf(stderr, "accept: %s\n",
          get_error_string(get_socket_error(), NULL, 0));
      break;
    }

    printf("Client connected: %s\n", inet_ntoa(client_addr.sin_addr));
    close_socket(client_sock);
  }

  close_socket(server_sock);

  #ifdef _WIN32
    WSACleanup();
  #endif
}
