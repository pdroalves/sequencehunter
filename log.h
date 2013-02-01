#ifndef LOG_H
#define LOG_H
void prepareLog(char *tempo);
void print_time();
void printToLog_Fim(int tiro,int raio,int fim,float x,float z);
void printToLog_Mem(float MemUsada,float MemPorIter);
void printToLog_Runtime(float t);
void printStringInt(char *c,int n);
void printString(char*,char*);
void printSet(int);
void print_open_file(char *c);
void print_total_seqs(int sensos,int antisensos);
void print_despareadas(char *seq,int sensos,int antisensos);
void print_despareadas_seqs(int sensos,int antisensos);
void print_tipos_seqs(int sensos,int antisensos);
void print_seqs_carregadas(int);
void print_resultados(lista_ligada** resultados);
void print_tempo(float tempo);
void print_tempo_optional(float tempo);
void print_sequencias_validas(int seqs_validas);
void print_resultado(lista_ligada *p);
void print_string_float(char *s,float f);
void print_string_float_optional(char *s,float f);
void print_seqs_processadas(int total,int sensos,int antisensos);
void print_seqs_processadas_optional(int total,int sensos,int antisensos);
void print_seqs_filas(int sensos,int antisensos);
void print_seqs_filas_optional(int sensos,int antisensos);
void closeLog();
#endif
