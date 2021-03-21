#define MAXSIZE	256
enum opcode {INIT, SET, GET, MOD, DEL ,EXIST, ITEMS};

struct request_struct  {
	int op;

	char key[MAXSIZE];
	char v1[MAXSIZE];
	int v2;
	float v3;

	char *pv1;
	int *pv2;
	int *pv3;

	char q_name[MAXSIZE];
};

typedef struct request_struct *request;
