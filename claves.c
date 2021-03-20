#include <mqueue.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include "request.h"

int comm_up = 0;
mqd_t server_q;
mqd_t client_q;

int init_comm(){

  // Atributos de la cola del cliente
  attr.mq_maxmsg = 1;
  attr.mq_msgsize = sizeof(request);

  // Nombre de la cola cliente = client-PID
  sprintf(q_name,  "/client-%d", getpid());
  client_q = mq_open(q_name, O_CREAT|O_RDONLY, 0700, &attr);
  if (client_q == -1) {
    perror("Error estableciendo la comunicación con el servior: fallo en la cola del cliente.");
    return -1;
  }
  server_q = mq_open("/server_q", O_WRONLY);
  if (server_q == -1){
    mq_close(client_q);
    perror("Error estableciendo la comunicación con el servior: fallo en la cola del servidor.");
    return -1;
  }

  return 0;
}

int init(){
  // Inicio de la comunicación
  if (comm_up == 0){
    if (init_comm() == -1){
      return -1;
    }
  }

  // Elaboracion de la peticion
  request req;
  req.op = INIT;
  req.key = NULL;
  req.v3 = NULL;
  req.v2 = NULL;
  req.v3 = NULL;
  strcpy(req.q_name, client_q);

  // Envio de la peticion
  if (mq_send(server_q, (const char *) &req, sizeof(request), 0) < 0){
    perror("Error enviando la petición desde el cliente");
    return -1;
  }

  // Recepcion de la peticion
  int res;
  if (mq_receive(client_q, (char *) &res, sizeof(int), 0) < 0){
    perror("Error recibiendo la respuesta del servidor en el cliente");
    return -1;
  }

  // Impresión de la respuesta
  if (res == -1){
    printf("Error inicializando la lista de tuplas.\n");
  } else {
    printf("Lista de tupas inicializada correctamente.\n");
  }

  return res;
}

int set_value(char *key, char *v1, int *v2, float *v3){
  // Inicio de la comunicación
  if (comm_up == 0){
    if (init_comm() == -1){
      return -1;
    }
  }

  // Elaboracion de la peticion
  request req;
  req.op = SET;
  req.key = *key;
  req.v3 = *v1;
  req.v2 = *v2;
  req.v3 = *v3;
  strcpy(req.q_name, client_q);

  // Envio de la peticion
  if (mq_send(server_q, (const char *) &req, sizeof(request), 0) < 0){
    perror("Error enviando la petición desde el cliente");
    return -1;
  }

  // Recepcion de la peticion
  int res;
  if (mq_receive(client_q, (char *) &res, sizeof(int), 0) < 0){
    perror("Error recibiendo la respuesta del servidor en el cliente");
    return -1;
  }

  // Impresión de la respuesta
  if (res == -1){
    printf("Error añadiendo la tupla.\n");
    return -1;
  } else {
    printf("La tupla se ha añadido correctamente.\n");
    return 0;
  }
}

int get_value(char *key, char *v1, int *v2, float *v3){
  // Inicio de la comunicación
  if (comm_up == 0){
    if (init_comm() == -1){
      return -1;
    }
  }

  // Elaboracion de la peticion
  request req;
  req.op = GET;
  req.key = *key;
  req.v3 = *v1;
  req.v2 = *v2;
  req.v3 = *v3;
  strcpy(req.q_name, client_q);

  // Envio de la peticion
  if (mq_send(server_q, (const char *) &req, sizeof(request), 0) < 0){
    perror("Error enviando la petición desde el cliente");
    return -1;
  }

  // Recepcion de la peticion
  int res;
  if (mq_receive(client_q, (char *) &res, sizeof(int), 0) < 0){
    perror("Error recibiendo la respuesta del servidor en el cliente");
    return -1;
  }

  // Impresión de la respuesta
  if (res == -1){
    printf("Error leyendo la tupla.\n");
    return -1;
  } else {
    printf("La tupla se ha leido correctamente:\n");
    printf("- Clave: \"%s\"\n- V1: \"%s\"\n- V2: \"%d\"\n- V3: \"%f\"\n", *key, *v1, *v2, *v3);
    return 0;
  }
}

int modify_value(char *key, char *v1, int v2, float v3){
  // Inicio de la comunicación
  if (comm_up == 0){
    if (init_comm() == -1){
      return -1;
    }
  }

  // Elaboracion de la peticion
  request req;
  req.op = MOD;
  req.key = *key;
  req.v3 = *v1;
  req.v2 = *v2;
  req.v3 = *v3;
  strcpy(req.q_name, client_q);

  // Envio de la peticion
  if (mq_send(server_q, (const char *) &req, sizeof(request), 0) < 0){
    perror("Error enviando la petición desde el cliente");
    return -1;
  }

  // Recepcion de la peticion
  int res;
  if (mq_receive(client_q, (char *) &res, sizeof(int), 0) < 0){
    perror("Error recibiendo la respuesta del servidor en el cliente");
    return -1;
  }

  // Impresión de la respuesta
  if (res == -1){
    printf("Error modificando la tupla.\n");
    return -1;
  } else {
    printf("La tupla se ha modificado correctamente:\n");
    printf("- Clave: \"%s\"\n- V1: \"%s\"\n- V2: \"%d\"\n- V3: \"%f\"\n", *key, *v1, *v2, *v3);
    return 0;
}

int delete_key(char *key){
  // Inicio de la comunicación
  if (comm_up == 0){
    if (init_comm() == -1){
      return -1;
    }
  }

  // Elaboracion de la peticion
  request req;
  req.op = DEL;
  req.key = *key;
  req.v3 = NULL;
  req.v2 = NULL;
  req.v3 = NULL;
  strcpy(req.q_name, client_q);

  // Envio de la peticion
  if (mq_send(server_q, (const char *) &req, sizeof(request), 0) < 0){
    perror("Error enviando la petición desde el cliente");
    return -1;
  }

  // Recepcion de la peticion
  int res;
  if (mq_receive(client_q, (char *) &res, sizeof(int), 0) < 0){
    perror("Error recibiendo la respuesta del servidor en el cliente");
    return -1;
  }

  // Impresión de la respuesta
  if (res == -1){
    printf("Error eliminando la tupla.\n");
    return -1;
  } else {
    printf("La tupla se ha eliminado correctamente:\n");
    return 0;
}

int exists(char *key){
  // Inicio de la comunicación
  if (comm_up == 0){
    if (init_comm() == -1){
      return -1;
    }
  }

  // Elaboracion de la peticion
  request req;
  req.op = EXIST;
  req.key = *key;
  req.v3 = NULL;
  req.v2 = NULL;
  req.v3 = NULL;
  strcpy(req.q_name, client_q);

  // Envio de la peticion
  if (mq_send(server_q, (const char *) &req, sizeof(request), 0) < 0){
    perror("Error enviando la petición desde el cliente");
    return -1;
  }

  // Recepcion de la peticion
  int res;
  if (mq_receive(client_q, (char *) &res, sizeof(int), 0) < 0){
    perror("Error recibiendo la respuesta del servidor en el cliente");
    return -1;
  }

  // Impresión de la respuesta
  if (res == -1){
    printf("La clave no corresponde a ninguna tupla.\n");
    return -1;
  } else {
    printf("La clave ya existe en la lista.\n");
    return 0;
}

int num_items(){
  // Inicio de la comunicación
  if (comm_up == 0){
    if (init_comm() == -1){
      return -1;
    }
  }

  // Elaboracion de la peticion
  request req;
  req.op = ITEMS;
  req.key = NULL;
  req.v3 = NULL;
  req.v2 = NULL;
  req.v3 = NULL;
  strcpy(req.q_name, client_q);

  // Envio de la peticion
  if (mq_send(server_q, (const char *) &req, sizeof(request), 0) < 0){
    perror("Error enviando la petición desde el cliente");
    return -1;
  }

  // Recepcion de la peticion
  int res;
  if (mq_receive(client_q, (char *) &res, sizeof(int), 0) < 0){
    perror("Error recibiendo la respuesta del servidor en el cliente");
    return -1;
  }

  // Impresión de la respuesta
  if (res == -1){
    printf("Error leyendo la tupla.\n");
    return -1;
  } else {
    printf("Actualmente hay %d tuplas en la lista.\n", res);
    return 0;
}
