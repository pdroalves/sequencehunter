#ifndef LOG_H
#define LOG_H
typedef int gboolean;
void prepareLog(char *tempo,gboolean gui);
void print_open_file(char *c);
void printStringInt(char *c,int n);
void printString(char *c,char *s);
void printSet(int n);
void print_total_seqs(int sensos,int antisensos);
void print_despareadas_seqs(int sensos,int antisensos);
void print_despareadas(char *seq,int sensos,int antisensos);
void print_tipos_seqs(int sensos,int antisensos);
void print_tempo(float tempo);
void print_tempo_optional(float tempo);
void print_resultado(lista_ligada *p);
void print_string_float(char *s,float f);
void print_seqs_processadas(int total,int sensos,int antisensos);
void print_seqs_filas(int sensos,int antisensos);
void print_seqs_filas_optional(int sensos,int antisensos);
void closeLog();
#endif
