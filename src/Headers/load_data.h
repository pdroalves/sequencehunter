#ifndef LOAD_DATA_H
#define LOAD_DATA_H
int open_file(char **entrada,int qnt,int silent);
void close_file();
void prepare_buffer(struct buffer *,int);
int fill_buffer(char **seqs,int MAX_TO_LOAD,int *SEQS_LOADED);
int get_setup();
int check_seq(char *seq,int *bloco1,int *bloco2,int *blocoV);
void close_buffer(Buffer *b);
#endif
