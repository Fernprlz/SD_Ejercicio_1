
// TODO: Son todas necesarias?

#include <mqueue.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include "mensaje.h"

// MENSAJE.H ESTA INCLUIDA, ASI QUE TENEMOS ACCESO A LAS CONSTANTES DE LOS OPCODES

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
