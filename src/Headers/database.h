#ifndef DATABASE_H
#define DATABASE_H
void db_start_transaction();
void db_commit_transaction();
void sh_db_create(char *filename);
void db_add(char *seq_central,char *seq_cincoL,int tipo);
void db_destroy();
#endif
