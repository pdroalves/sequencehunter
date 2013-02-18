#ifndef DATABASE_H
#define DATABASE_H
typedef struct ham_db_t ham_db_t;
ham_db_t* ham_create_db(char *filename,const int key_max_size);
void ham_add(ham_db_t* db,char *seq_central,char *seq_cincoL,char *tipo);
void ham_destroy(ham_db_t* db);
#endif
