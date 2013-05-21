#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H
void criar_db_manager(char *output_dir,char *tempo,const int key_max_size,int silent);
void adicionar_db(char *central,char *cincol,int tipo);
void destroy_db_manager();
char* get_database_filename();
#endif
