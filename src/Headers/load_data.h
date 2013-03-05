#ifndef LOAD_DATA_H
#define LOAD_DATA_H
int open_file(char **entrada,int qnt,int silent);
char *itoaa(unsigned long num);
int cuda_supported();
void close_file();
void prepare_buffer(struct buffer *,int);
int check_seq(char *,int *,int *,int *);
int fill_buffer(char **seqs,int MAX_TO_LOAD,int *SEQS_LOADED);
int get_setup();
int check_gpu_mode();
int check_sequencias_validas(int silent);
extern int check_seqs;
void despejar_seq(char *seq,FILE *f);
char* carrega_do_arquivo(int n,FILE *filename);
#endif
