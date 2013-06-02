#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <omp.h>
#include <sqlite3.h>
#include "../Headers/operacoes.h"
#include "../Headers/estruturas.h"
#include "../Headers/load_data.h"

#define GIGA 1073741824 
#define DB_FREE_MEMORY_FACTOR 0.90
#define q_size 500
#define DATABASE ":memory:"

double MAX_DB_MEM_USE;
// Create a db for database connection, create a pointer to sqlite3
sqlite3 *db;
// The number of query to be dbd,size of each query and pointer
int count;
int destroyed;
sqlite3_stmt *stmt_insert;
char *database_path;

int get_hash(char* original){
	char c;
	int hash = 5381;

	while(c = *original++)
		hash = ((hash << 5)+hash) + c;
	return hash;
}

void db_start_transaction(){
    char * sErrMsg;
    /*			printf("###########################################\n");
			printf("###########################################\n");
			printf("################### NOVA TRANSACTION ###### \n");
			printf("###########################################\n");
			printf("###########################################\n");
    */
	sqlite3_exec(db, "BEGIN TRANSACTION", 0, 0, &sErrMsg);
	return;
}

void db_commit_transaction(){
	char *sErrMsg;
	sqlite3_exec(db, "COMMIT TRANSACTION", 0, 0, &sErrMsg);
	if(sqlite3_memory_used() > MAX_DB_MEM_USE){
		printf("Cleaning database...\n");
		sqlite3_exec(db,"vacuum", NULL,NULL,&sErrMsg);
	}
	return;
}

void db_create(char *filename){
	int ret;
    char * sErrMsg;
	char *query;
    sqlite3_stmt *stmt;
    destroyed = 0;

    database_path = filename;
    
    MAX_DB_MEM_USE = (double)getTotalSystemMemory()*DB_FREE_MEMORY_FACTOR*1024;
	
	ret = sqlite3_open(filename,&db);
	
    if (!db)
        printf("Not sure why, but the database didn't open.\n");
        
	// If connection failed, db returns NULL
	if(ret)
	{
		printf("Database connection failed - %s\n",filename);
		exit(1);
	}
	

	// Setup DB
	sqlite3_exec(db,"PRAGMA synchronous = OFF", NULL,NULL,&sErrMsg);
	if(sErrMsg != NULL){
		printf("Pragma error: %s\n",sErrMsg);
		exit(1);
	}
	sqlite3_exec(db,"PRAGMA journal_mode = MEMORY",NULL,NULL,&sErrMsg);
	if(sErrMsg != NULL){
		printf("Pragma error: %s\n",sErrMsg);
		exit(1);
	}	
	/*sqlite3_exec(db,"PRAGMA page_size = 1024",NULL,NULL,&sErrMsg);
	if(sErrMsg != NULL){
		printf("Pragma error: %s\n",sErrMsg);
		exit(1);
	}
	sqlite3_exec(db,"PRAGMA cache_size = 5000000",NULL,NULL,&sErrMsg);
	if(sErrMsg != NULL){
		printf("Pragma error: %s\n",sErrMsg);
		exit(1);
	}*/
	sqlite3_exec(db,"PRAGMA shrink_memory",NULL,NULL,&sErrMsg);
	if(sErrMsg != NULL){
		printf("Pragma error: %s\n",sErrMsg);
		exit(1);
	}		
	/*sqlite3_exec(db,"PRAGMA foreign_key = ON",NULL,NULL,&sErrMsg);
	if(sErrMsg != NULL){
		printf("Pragma error: %s\n",sErrMsg);
		exit(1);
	}*/	
	
		
	query = (char*)malloc(500*sizeof(char));
	
	// Create the SQL query for creating a table
	strcpy(query,"CREATE TABLE events_tmp (id INTEGER NOT NULL,main_seq TEXT,senso INTEGER DEFAULT 0,antisenso INTEGER DEFAULT 0,PRIMARY KEY(id))");
	// Execute the query for creating the table
	ret = sqlite3_exec(db,query,NULL, NULL,&sErrMsg);
	if(sErrMsg != NULL){
		printf("DB error: %s - %s\n",sErrMsg,filename);
		exit(1);
	}

	count =0;
	
	// Main
	//
	// Compile insert-statement
	sprintf(query, "INSERT OR IGNORE INTO events_tmp (main_seq,senso,antisenso) VALUES (@SEQ,@SEN,@ANT)");
	ret = sqlite3_prepare_v2(db,  query, -1, &stmt_insert, 0);
	if(ret != SQLITE_OK){
		printf("Error on statement compile 1b - %d.\n",ret);
		exit(1);
	}

	
	sqlite3_soft_heap_limit64(MAX_DB_MEM_USE);
	
	destroyed = 0;

	free(query);
	return;
}

void db_add(char *seq_central,char *seq_cincoL,int tipo){
	int ret=0;
	int id;
    char * sErrMsg;

    sqlite3_bind_text(stmt_insert,1,seq_central,-1,SQLITE_TRANSIENT);

    if(tipo == SENSO){
		// Atualiza contagem senso da seq_central
		
   		sqlite3_bind_int(stmt_insert,2,1);
   		sqlite3_bind_int(stmt_insert,3,0);
        
	}else{
		// Atualiza contagem antisenso da seq_centrall
		
   		sqlite3_bind_int(stmt_insert,2,0);
   		sqlite3_bind_int(stmt_insert,3,1);
	}

	ret = sqlite3_step(stmt_insert);
	sqlite3_clear_bindings(stmt_insert);
	sqlite3_reset(stmt_insert);
	if(ret != SQLITE_DONE){
		printf("Error on SQLite step 4 - %d. => %s\n",ret,seq_central);
		exit(1);
	}
	count++;
	return;
}

void db_destroy(){
    char * sErrMsg;
	int ret;
	char createEventsQuery[] = "CREATE TABLE events as SELECT main_seq,SUM(senso) qnt_sensos,SUM(antisenso) qnt_antisensos,min(SUM(senso),SUM(antisenso)) pares FROM events_tmp GROUP BY main_seq";
	char dropTmpQuery[] = "DROP TABLE events_tmp";
	
	if(!destroyed){
		db_start_transaction();
		ret = sqlite3_exec(db,createEventsQuery,NULL, NULL,&sErrMsg);
		db_commit_transaction();
		if(sErrMsg == NULL){
			db_start_transaction();
			ret = sqlite3_exec(db,dropTmpQuery,NULL, NULL,&sErrMsg);
			db_commit_transaction();
			ret = sqlite3_exec(db,"vacuum",NULL, NULL,&sErrMsg);	
		}else{
			if(ret == SQLITE_FULL)
		 	 printf("Database ERROR! %s\nPlease, free up some hard disk space and run Sequence Hunter again passing '%s --fixdb' as parameter.\n",sErrMsg,database_path);
		 	else
		 		 printf("Database ERROR! %s\n",sErrMsg);
		}
		
		sqlite3_finalize(stmt_insert);
		sqlite3_close(db);
		destroyed = 1;
	}
}


/*int callback_phase_one(void *NotUsed,int argc,char **argv,char **azColName){

}

int callback_phase_one(void *NotUsed,int argc,char **argv,char **azColName){
	// Verifica se tabela events existe
	int i;
  if(argc == 0){
  	// Tabela events nao existe

  }else{
  	// Tabela events existe
  }
  return 0;

}
*/
void db_fix(char *filename){
	char *db_err;
	int ret;
	char *sErrMsg;
	char createEventsQuery[] = "CREATE TABLE events as SELECT main_seq,SUM(senso) qnt_sensos,SUM(antisenso) qnt_antisensos,min(SUM(senso),SUM(antisenso)) pares FROM events_tmp GROUP BY main_seq";
	char dropTmpQuery[] = "DROP TABLE events_tmp";

	ret = sqlite3_open(filename,&db);
    
    if (!db)
        printf("Not sure why, but the database didn't open.\n");

		db_start_transaction();
		ret = sqlite3_exec(db,createEventsQuery,NULL, NULL,&sErrMsg);
		db_commit_transaction();
		if(sErrMsg == NULL){
			db_start_transaction();
			ret = sqlite3_exec(db,dropTmpQuery,NULL, NULL,&sErrMsg);
			db_commit_transaction();
			ret = sqlite3_exec(db,"vacuum",NULL, NULL,&sErrMsg);	
		}else{
			if(ret == SQLITE_FULL)
		 	 printf("Database ERROR! %s\nPlease, free up some hard disk space and run Sequence Hunter again passing '%s --fixdb' as parameter.\n",sErrMsg,database_path);
		 	else
		 		 printf("Database ERROR! %s\n",sErrMsg);
		}
		
		sqlite3_close(db);
	return;
}
