#ifndef GHASHTABLE_H
#define GHASHTABLE_H
void load_from_tmp_file();
void open_and_load_file(char *filename);
void criar_ghash_table(char *tempo,const int key_max_size);
void adicionar_ht(char *central,char *cincol,int tipo);
void destroy_ghash_table();
void process_signal_ht();
#endif
