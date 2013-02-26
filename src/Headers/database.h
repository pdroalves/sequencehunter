#ifndef DATABASE_H
#define DATABASE_H
void db_init_lock();
void db_start_transaction();
void db_commit_transaction();
void db_create(char *filename,const int key_max_size);
void db_add(char *seq_central,char *seq_cincoL,int tipo);
void db_destroy();
#endif
