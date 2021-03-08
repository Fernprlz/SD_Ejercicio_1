#include <mqueue.h>
#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "request.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////// S E R V I C I O S ///////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Variables de control de capacidad del servidor
#define MAX_THREADS 	10
#define MAX_PETICIONES 	256

// Cola del servidor
mqd_t server_q;

// Variables del buffer de peticiones
struct peticion buffer_peticiones[MAX_PETICIONES];
int n_elementos;
int pos = 0;

// MUTEX de condiciones de estado de la cola
pthread_mutex_t mutex;
pthread_cond_t no_lleno;
pthread_cond_t no_vacio;

// MUTEX de control de estado de los threads
pthread_mutex_t mfin;
int fin = false;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////// S E R V I C I O S ///////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int init(){
  // 1. destruir tuplas existentes
  // 2. meter nuevo valor
}

int set_value(){
  // 1. comprobar que existe
  // 2. meter nuevo valor
}

int get_value(){
  // 1. comprobar que existe
  // 2. devolver nuevo valor
}

int modify_value(){
  // 1. comprobar que existe
  // 2. modificar valor
}

int delete_key(){
  // 1. comprobar que existe
  // 2. borrar elemento
}

int exists(){
  // 1. buscar si existe
}

int num_items(){
  // devolver el número de tuplas
}

////////////////////////////////////////////////////////////////////////////////

void servicio(void ){
  // Peticion local
  struct peticion request;
  // Cola del cliente
  mqd_t client_q;
  // Resultado del servicio
  int res;

	while(true){

		pthread_mutex_lock(&mutex);

    // Esperar a recibir peticiones
		while (n_elementos == 0) {
      // Evaluar estado de ejecución del servidor
      if (fin == true) {
				fprintf(stderr,"Finalizando servicio\n");
				pthread_mutex_unlock(&mutex);
				pthread_exit(0);
			}
			pthread_cond_wait(&no_vacio, &mutex);
		}

    // Recoger la petición del buffer
		request = buffer_peticiones[pos];
    // Cambia el puntero de buffer
		pos = (pos + 1) % MAX_PETICIONES;
		n_elementos--;
    // Señales pertinentes a los threads
		pthread_cond_signal(&no_lleno);
		pthread_mutex_unlock(&mutex);

		// Procesado de la peticion

		/* ejecutar la petición del cliente y preparar respuesta */
    switch(request.op){
      case INIT:
        res = init();
        break;
      case SET:
        res = set_value();
        break;
      case GET:
        res = get_value();
        break;
      case MOD:
        res = modify_value();
        break;
      case DEL:
        res = delete_key();
        break;
      case EXIST:
        res = exist();
        break;
      case ITEMS:
        res = num_items();
        break;
      default:
        res = -1
        perror("Código de operación invalido")
        break;
    }

		// Respuesta a la cola del cliente
		client_q = mq_open(request.q_name, O_WRONLY);
		if (client_q == -1)
			perror("No se puede abrir la cola del cliente");
		else {
			mq_send(client_q, (const char *) &res, sizeof(int), 0);
			mq_close(client_q);
		}
	}
 	pthread_exit(0);
}

////////////////////////////////////////////////////////////////////////////////
///////////////////// E J E C U C I O N - S E R V I D O R //////////////////////
////////////////////////////////////////////////////////////////////////////////

int main(void) {
  // Cola de mensajes
	mqd_t server_q;
  struct mq_attr attr;
  attr.mq_maxmsg = 10;
	attr.mq_msgsize = sizeof(struct peticion);

  // Peticion actual
	struct peticion request;

	// Threads
	pthread_attr_t t_attr;
	pthread_t thid[MAX_THREADS];
	int error;
	int pos = 0;

  // Inicialización de la cola del server
  server_q = mq_open("/SERVIDOR_COLA_MENSAJES", O_CREAT|O_RDONLY, 0700, &attr);
	if (server_q == -1) {
		perror("Fallo al crear la cola del servidor");
		return -1;
	}

  // Inicialización de MUTEX de la cola y la pool
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&no_lleno, NULL);
	pthread_cond_init(&no_vacio, NULL);
	pthread_mutex_init(&mfin, NULL);

  // Creación de la pool de threads
	pthread_attr_init(&t_attr);
	for (int ii = 0; ii < MAX_THREADS; ii++){
		if (pthread_create(&thid[ii], NULL, (void *) servicio, NULL) !=0){
			perror("Error creando la pool de threads\n");
			return 0;
		}
  }

  // Bucle de ejecución del servidor
	while (true) {
    // Recepción de un request
		error = mq_receive(server_q, (char *) &request, sizeof(struct peticion), 0);
		if (error == -1){
			perror("Error en la recepción de un request");
			break;
		}

    // Esperar a que se desocupe algun thread
		pthread_mutex_lock(&mutex);
		while (n_elementos == MAX_PETICIONES){
      pthread_cond_wait(&no_lleno, &mutex);
    }
		buffer_peticiones[pos] = request; // Recibe la peticion en la cola de peticiones
		pos = (pos + 1) % MAX_PETICIONES; // Mueve el puntero de posición del buffer de peticiones al siguiente hueco libre
		n_elementos++;
		pthread_cond_signal(&no_vacio); // Avisa al resto de threads parados por el cond_wait
		pthread_mutex_unlock(&mutex); // Libera el mutex global
	}

  // Ejecución del servidor terminada
	pthread_mutex_lock(&mfin);
	fin = true;
	pthread_mutex_unlock(&mfin);

	pthread_mutex_lock(&mutex);
	pthread_cond_broadcast(&no_vacio);
	pthread_mutex_unlock(&mutex);

  // Recogida de threads
	for (int i = 0; i < MAX_THREADS; i++){
		pthread_join(thid[i], NULL);
  }

  // Limpieza
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&no_lleno);
	pthread_cond_destroy(&no_vacio);
	pthread_mutex_destroy(&mfin);

	return 0;
}
