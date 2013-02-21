#ifndef DATABASE_H
#define DATABASE_H
void db_create(char *filename,const int key_max_size);
void db_add(char *seq_central,char *seq_cincoL,char *tipo);
void db_destroy();
#endif
