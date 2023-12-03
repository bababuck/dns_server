#include <ifaddrs.h>
#include <string.h>

#include "../include/server.h"

int setup_server(uint16_t port_num, int protocol, bool bind_socket) {
  int socket_info = socket(AF_INET, protocol, 0);
  if (socket_info < 0) {
    perror("socket");
    exit(1);
  }
  if (!bind_socket) {
    return socket_info;
  }

  sockaddr_in_t server;
  server.sin_addr.s_addr = INADDR_ANY; // Any addresses
  server.sin_family = AF_INET; // Internet Protocol v4 addresses
  server.sin_port = htons(port_num);

  // Bind socket to requested port
  if (bind(socket_info, (sockaddr_t *) &server, sizeof(server)) < 0) {
    perror("bind");
    exit(2);
  }

  socklen_t length = sizeof(sockaddr_in_t);
  if (getsockname(socket_info, (sockaddr_t *) &server, &length) < 0) {
    perror("getsockname");
    exit(3);
  }

  if (server.sin_port != htons(port_num)) {
    perror("port_num unavailable");
    exit(4);
  }

  return socket_info;
}

uint8_t send_packet(uint16_t port_num, char *ip, int socket_info, uint8_t *message, uint8_t message_len) {
  //  printf("Sending Packet: Port: %d Ip: %s Socket: %d Bytes: %d\n", port_num, ip, socket_info, message_len);
  sockaddr_in_t dest_server;
  dest_server.sin_family  = AF_INET;  // Internet Domain
  dest_server.sin_port = htons(port_num); // Server Port
  dest_server.sin_addr.s_addr = inet_addr(ip); // Server's Address

  if (sendto(socket_info, message, message_len, 0, (sockaddr_t *) &dest_server, sizeof(dest_server)) < 0) {
    perror("sendto()");
    exit(5);
  }
  //  printf("Sent from %d\n", socket_info);
  return 0;
}

uint8_t setup_response_thread(pthread_t *thread, void *(receive_routine)(void*), void *arg) {
  if (pthread_create(thread, NULL, receive_routine, arg) != 0) {
    perror("pthread_create");
    exit(6);
  }
  return 0;
}

uint8_t kill_response_thread(pthread_t *thread) {
  if (pthread_cancel(*thread) != 0) {
    perror("pthread_kill");
    exit(7);
  }
  sleep(2);
  return 0;
}

uint8_t recieve_message(uint8_t *buffer, uint8_t length, int socket_info) {
  //  printf("Recieving Packet: Socket: %d\n", socket_info);
  uint8_t bytes_recieved;
  sockaddr_in_t client;
  int len = sizeof(client);
  if((bytes_recieved = recvfrom(socket_info, buffer, length, 0, (sockaddr_t *)&client, (socklen_t *)&len)) < 0) {
    perror("recvfrom");
    exit(5);
  }
  //  printf("Recieved at socket: %d Bytes: %d\n", socket_info, bytes_recieved);
  return bytes_recieved;
}

char* get_ip() {
  struct ifaddrs * ifAddrStruct=NULL;
  struct ifaddrs * ifa=NULL;
  void * tmpAddrPtr=NULL;

  getifaddrs(&ifAddrStruct);
  for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
    if (!ifa->ifa_addr) {
      continue;
    }
    if (ifa->ifa_addr->sa_family == AF_INET) { // IP4 Address
      tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
      char addressBuffer[INET_ADDRSTRLEN];
      inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
      if (strcmp(ifa->ifa_name, "en0") == 0) {
        //	printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer);
	char *ip = strdup(addressBuffer);
	freeifaddrs(ifAddrStruct);
	return ip;
      }
    }
  }
  if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);
  return NULL;
}

int create_tcp_connections(int tcp_socket) {
  // Wait until connection is seed
  if (listen(tcp_socket, 1) != 0) {
    perror("Listen()");
    exit(4);
  }

  // Accept connection
  sockaddr_in_t client;
  int new_socket;
  unsigned int namelen = sizeof(client);
  if ((new_socket = accept(tcp_socket, (sockaddr_t *) &client, &namelen)) == -1) {
    perror("Accept()");
    exit(5);
  }
  return new_socket;
}

uint8_t connect_to_tcp(int tcp_socket, char *ip, uint16_t port_num) {
  sockaddr_in_t dest_server;
  dest_server.sin_family  = AF_INET;  // Internet Domain
  dest_server.sin_port = htons(port_num); // Server Port
  dest_server.sin_addr.s_addr = inet_addr(ip); // Server's Address

  if (connect(tcp_socket, (sockaddr_t*) &dest_server, sizeof(dest_server)) < 0) {
    perror("Connect()");
    exit(4);
  }
  return 0;
}
