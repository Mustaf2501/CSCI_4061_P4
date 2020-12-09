#ifndef _REENTRANT
#define _REENTRANT
#endif

#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "util.h"
#define MSGSIZE 128

int sock; 

/**********************************************
 * init
   - port is the number of the port you want the server to be
     started on
   - initializes the connection acception/handling system
   - YOU MUST CALL THIS EXACTLY ONCE (not once per thread,
     but exactly one time, in the main thread of your program)
     BEFORE USING ANY OF THE FUNCTIONS BELOW
   - if init encounters any errors, it will call exit().
************************************************/
void init(int port) {
  int sockfd; // fd for main socket

  // some things for binding step 
  struct sockaddr_in my_addr; /* my address */
  //struct sockaddr_in client_addr; /* client's address */ //not sure if used 
  //int addr_size;
  int enable = 1;

  // CREATE SOCKET

   /* --- Create the socket --- */
  if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
    perror("Can't create socket");
    exit(1);
  } 

  // BIND SOCKET

   /* --- Bind the socket, making its address reusable --- */
  my_addr.sin_family = AF_INET;
  my_addr.sin_port = htons(port);
  my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  /* Note the use of INADDR_ANY to get the OS to fill in the local
     host address. You may not want to do this on a multi-home
     host. */

  /* Set the "re-use" option so we can shut down and restart the
     server without waiting for the standard timeout. */
  if (setsockopt(sockfd,SOL_SOCKET, SO_REUSEADDR, 
      &enable, sizeof(int)) == -1) {
    perror("Can't set socket option");
    exit(1);
  }

  if (bind(sockfd, (struct sockaddr *)&my_addr,
	           sizeof(my_addr)) == -1) {
    perror("Could not bind");
    exit(1);
  }

  // LISTEN

    /* --- Set up the socket to listen for incoming connection requests --- */
  if (listen(sockfd, 5) == -1) {
    perror("Could not listen");
    exit(1);
  }

  sock = sockfd; 

}

/**********************************************
 * accept_connection - takes no parameters
   - returns a file descriptor for further request processing.
     DO NOT use the file descriptor on your own -- use
     get_request() instead.
   - if the return value is negative, the request should be ignored.
***********************************************/
int accept_connection(void) {
  int client_fd;
  int addr_size;

  struct sockaddr_in client_addr; /* client's address */
  addr_size = sizeof(client_addr); 

  if (( client_fd = accept(sock, (struct sockaddr *)&client_addr, &addr_size)) == -1){
    perror("Failed to accept connection");
   return -1;
  }
  
  //fprintf(stderr, "server: client connection from %s\n", inet_ntoa(client_addr.sin_addr));

  //close(client_fd);
  
  return client_fd;

}

/**********************************************
 * get_request
   - parameters:
      - fd is the file descriptor obtained by accept_connection()
        from where you wish to get a request
      - filename is the location of a character buffer in which
        this function should store the requested filename. (Buffer
        should be of size 1024 bytes.)
   - returns 0 on success, nonzero on failure. You must account
     for failures because some connections might send faulty
     requests. This is a recoverable error - you must not exit
     inside the thread that called get_request. After an error, you
     must NOT use a return_request or return_error function for that
     specific 'connection'.
************************************************/
int get_request(int fd, char *filename) {

  char msg[MSGSIZE];
  int readsz = 0;

  if ((readsz = read(fd, msg, 128)) >= 0) {
    msg[readsz] = '\0';
    fprintf(stderr, "Client sez %s", msg);
  } else {
    perror ("server read problem");
    return -1;
  }

  //close(fd);
  return 0;
}

/**********************************************
 * return_result
   - returns the contents of a file to the requesting client
   - parameters:
      - fd is the file descriptor obtained by accept_connection()
        to where you wish to return the result of a request
      - content_type is a pointer to a string that indicates the
        type of content being returned. possible types include
        "text/html", "text/plain", "image/gif", "image/jpeg" cor-
        responding to .html, .txt, .gif, .jpg files.
      - buf is a pointer to a memory location where the requested
        file has been read into memory (the heap). return_result
        will use this memory location to return the result to the
        user. (remember to use -D_REENTRANT for CFLAGS.) you may
        safely deallocate the memory after the call to
        return_result (if it will not be cached).
      - numbytes is the number of bytes the file takes up in buf
   - returns 0 on success, nonzero on failure.
************************************************/
int return_result(int fd, char *content_type, char *buf, int numbytes) {
  return 0;
}

/**********************************************
 * return_error
   - returns an error message in response to a bad request
   - parameters:
      - fd is the file descriptor obtained by accept_connection()
        to where you wish to return the error
      - buf is a pointer to the location of the error text
   - returns 0 on success, nonzero on failure.
************************************************/
int return_error(int fd, char *buf) {
  return 0;
}
