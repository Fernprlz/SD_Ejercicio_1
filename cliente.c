#include <mqueue.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include "mensaje.h"

int main(void) {
  mqd_t server_q;
  mqd_t client_q;


  struct peticion request;
  int res;
  struct mq_attr attr;
  char q_name[MAXSIZE];

  attr.mq_maxmsg = 1;
  attr.mq_msgsize = sizeof(int);

  sprintf(q_name,  "/Cola-%d", getpid());
  client_q = mq_open(q_name, O_CREAT|O_RDONLY, 0700, &attr);
  if (client_q == -1) {
    perror("mq_open 1");
    return -1;
  }
  server_q = mq_open("/SERVIDOR_COLA_MENSAJES", O_WRONLY);
  if (server_q == -1){
    mq_close(client_q);
    perror("mq_open 2");
    return -1;
  }


  /* se rellena la petición */
  request.op = 0;
  request.a = 5;
  request.b = 2;
  strcpy(request.q_name, q_name);

  if (mq_send(server_q, (const char *) &request, sizeof(request), 0) < 0){
    perror("Error enviando la petición desde el cliente");
    return -1;
  }
  if (mq_receive(client_q, (char *) &res, sizeof(int), 0) < 0){
    perror("Error recibiendo la respuesta del servidor en el cliente");
    return -1;
  }

  printf("Resultado = %d\n", res);

  mq_close(server_q);
  mq_close(client_q);
  mq_unlink(q_name);
  return 0;
}
