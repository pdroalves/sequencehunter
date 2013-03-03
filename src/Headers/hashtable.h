#ifndef GHASHTABLE_H
#define GHASHTABLE_H
void criar_ghash_table(char *output_dir,char *tempo,const int key_max_size);
void adicionar_ht(char *central,char *cincol,int tipo);
void destroy_ghash_table();
char* get_database_filename();
#endif
