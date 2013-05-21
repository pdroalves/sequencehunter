#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <omp.h>
#include "sqlite3.h"
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
sqlite3_stmt *stmt_insert_main;
sqlite3_stmt *stmt_insert;
sqlite3_stmt *stmt_update_senso;
sqlite3_stmt *stmt_update_antisenso;
sqlite3_stmt *stmt_select_cincol;
sqlite3_stmt *stmt_insert_cincol;
sqlite3_stmt *stmt_insert_link;
sqlite3_stmt *stmt_update_senso_cincol;
sqlite3_stmt *stmt_update_antisenso_cincol;

unsigned long get_hash(char* original){
	char *c;
	unsigned long hash = 5381;

	while(c = *original++)
		hash = ((hash << 5)+hash) + c;
	return hash;
}

void db_start_transaction(){
    char * sErrMsg;
	/*		printf("###########################################\n");
			printf("###########################################\n");
			printf("################### NOVA TRANSACTION ###### \n");
			printf("###########################################\n");
			printf("###########################################\n");*/
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
	//sqlite3_exec(db,"PRAGMA page_size = 2048",NULL,NULL,&sErrMsg);
	if(sErrMsg != NULL){
		printf("Pragma error: %s\n",sErrMsg);
		exit(1);
	}
	sqlite3_exec(db,"PRAGMA cache_size = 5000000",NULL,NULL,&sErrMsg);
	if(sErrMsg != NULL){
		printf("Pragma error: %s\n",sErrMsg);
		exit(1);
	}
	sqlite3_exec(db,"PRAGMA shrink_memory",NULL,NULL,&sErrMsg);
	if(sErrMsg != NULL){
		printf("Pragma error: %s\n",sErrMsg);
		exit(1);
	}		
	sqlite3_exec(db,"PRAGMA foreign_key = ON",NULL,NULL,&sErrMsg);
	if(sErrMsg != NULL){
		printf("Pragma error: %s\n",sErrMsg);
		exit(1);
	}	
	
		
	query = (char*)malloc(500*sizeof(char));
	
	strcpy(query,"CREATE TABLE main_sequences (id INTEGER,sequence TEXT NOT NULL UNIQUE,PRIMARY KEY(id,sequence))");
	// Execute the query for creating the table
	ret = sqlite3_exec(db,query,NULL, NULL,&sErrMsg);
	if(sErrMsg != NULL){
		printf("DB error: %s - %s\n",sErrMsg,filename);
		exit(1);
	}
	
	// Create the SQL query for creating a table
	strcpy(query,"CREATE TABLE events (id INTEGER NOT NULL,fk_main_seq INTEGER NOT NULL UNIQUE,qnt_sensos INTEGER DEFAULT 0,qnt_antisensos INTEGER DEFAULT 0,pares INTEGER DEFAULT 0,PRIMARY KEY(id),FOREIGN KEY(fk_main_seq) REFERENCES main_sequences(id) ON UPDATE CASCADE ON DELETE CASCADE)");
	// Execute the query for creating the table
	ret = sqlite3_exec(db,query,NULL, NULL,&sErrMsg);
	if(sErrMsg != NULL){
		printf("DB error: %s - %s\n",sErrMsg,filename);
		exit(1);
	}
	// Create the SQL query for creating a table
	//strcpy(query,"CREATE TABLE events_cincol (id INTEGER NOT NULL,cincol_seq TEXT NOT NULL UNIQUE,PRIMARY KEY(id))");
	// Execute the query for creating the table
	//ret = sqlite3_exec(db,query,NULL, NULL,&sErrMsg);
	//if(sErrMsg != NULL){
	//	printf("DB error %s\n",sErrMsg);
	//	exit(1);
	//}
	
	// Create the SQL query for creating a table
	//strcpy(query,"CREATE TABLE link (main_id INTEGER NOT NULL,cincol_id INTEGER NOT NULL,qnt_sensos INTEGER DEFAULT 0,qnt_antisensos INTEGER DEFAULT 0,pares INTEGER DEFAULT 0,UNIQUE(main_id,cincol_id),FOREIGN KEY(main_id) REFERENCES events(id),FOREIGN KEY(cincol_id) REFERENCES events_cincol(id))");

	// Execute the query for creating the table
	//ret = sqlite3_exec(db,query,NULL, NULL,&sErrMsg);
	//if(sErrMsg != NULL){
	//	printf("DB error %s\n",sErrMsg);
	//	exit(1);
	//}
	count =0;
	
	// Main
	//
	// Compile insert-statement
	sprintf(query, "INSERT OR IGNORE INTO main_sequences VALUES(@IDX,@SEQ)");
	ret = sqlite3_prepare_v2(db,  query, -1, &stmt_insert_main, 0);
	if(ret != SQLITE_OK){
		printf("Error on statement compile 1 - %d.\n",ret);
		exit(1);
	}
	
	// Compile insert-statement
	sprintf(query, "INSERT OR IGNORE INTO events (fk_main_seq) VALUES (@IDX)");
	ret = sqlite3_prepare_v2(db,  query, -1, &stmt_insert, 0);
	if(ret != SQLITE_OK){
		printf("Error on statement compile 1 - %d.\n",ret);
		exit(1);
	}

	// Sensos
	sprintf(query, "UPDATE events SET qnt_sensos=qnt_sensos+1 WHERE fk_main_seq = @IDX");
	ret = sqlite3_prepare_v2(db,  query, -1, &stmt_update_senso, 0);
	if(ret != SQLITE_OK){
		printf("Error on statement compile 2 - %d.\n",ret);
		exit(1);
	}
	
	// Antisensos
	sprintf(query, "UPDATE events SET qnt_antisensos=qnt_antisensos+1 WHERE fk_main_seq = @IDX");
	ret = sqlite3_prepare_v2(db,  query, -1, &stmt_update_antisenso, 0);
	if(ret != SQLITE_OK){
		printf("Error on statement compile 3 - %d.\n",ret);
		exit(1);
	}
	
	// CincoL
	//
	// Compile insert-statement
	/*sprintf(query, "INSERT OR IGNORE INTO events_cincol (cincol_seq) VALUES (@CSE)");
	ret = sqlite3_prepare_v2(db,  query, -1, &stmt_insert_cincol, 0);
	if(ret != SQLITE_OK){
		printf("Error on statement compile 4 - %d.\n",ret);
		exit(1);
	}
	
	// Link
	//	
	sprintf(query, "INSERT OR IGNORE INTO link (main_id,cincol_id) VALUES((SELECT id from events WHERE main_seq=@SEQ),(SELECT id from events_cincol WHERE cincol_seq=@NSE))");
	ret = sqlite3_prepare_v2(db,  query, -1, &stmt_insert_link, 0);
	if(ret != SQLITE_OK){
		printf("Error on statement compile 4 - %d.\n",ret);
		exit(1);
	}
	
	// Sensos
	sprintf(query, "UPDATE link SET qnt_sensos=qnt_sensos+1 WHERE  main_id=(SELECT id from events WHERE main_seq=@SEQ) AND cincol_id=(SELECT id from events_cincol WHERE cincol_seq=@NSE)");
	ret = sqlite3_prepare_v2(db,  query, -1, &stmt_update_senso_cincol, 0);
	if(ret != SQLITE_OK){
		printf("Error on statement compile 5 - %d.\n",ret);
		exit(1);
	}
	
	// Antisensos
	sprintf(query, "UPDATE link SET qnt_antisensos=qnt_antisensos+1 WHERE  main_id=(SELECT id from events WHERE main_seq=@SEQ) AND cincol_id=(SELECT id from events_cincol WHERE cincol_seq=@NSE)");
	ret = sqlite3_prepare_v2(db,  query, -1, &stmt_update_antisenso_cincol, 0);
	if(ret != SQLITE_OK){
		printf("Error on statement compile 6 - %d.\n",ret);
		exit(1);
	}*/
	
	sqlite3_soft_heap_limit64(MAX_DB_MEM_USE);
	
	destroyed = 0;

	free(query);
	return;
}

void db_add(char *seq_central,char *seq_cincoL,int tipo){
	int ret=0;
	int id;
    char * sErrMsg;
    unsigned long hash = get_hash(seq_central);
    
    // Insere seq_central
	sqlite3_bind_int(stmt_insert_main,1,hash);
	sqlite3_bind_text(stmt_insert_main,2,seq_central,-1,SQLITE_TRANSIENT);
	
	ret = sqlite3_step(stmt_insert_main);	
	
	sqlite3_clear_bindings(stmt_insert_main);
	sqlite3_reset(stmt_insert_main);
	if(ret != SQLITE_DONE){
		printf("Error on SQLite step 1a - %d. => %s\n",ret,seq_central);
		exit(1);
	}
	
	sqlite3_bind_int(stmt_insert,1,hash);
	
	ret = sqlite3_step(stmt_insert);	
	
	sqlite3_clear_bindings(stmt_insert);
	sqlite3_reset(stmt_insert);
	if(ret != SQLITE_DONE){
		printf("Error on SQLite step 1b - %d. => %s\n",ret,seq_central);
		exit(1);
	}
	
	/*if(seq_cincoL != NULL){
		// Insere seq_cincol
		sqlite3_bind_text(stmt_insert_cincol,1,seq_cincoL,-1,SQLITE_TRANSIENT);
		
		ret = sqlite3_step(stmt_insert_cincol);	
		
		sqlite3_clear_bindings(stmt_insert_cincol);
		sqlite3_reset(stmt_insert_cincol);
		if(ret != SQLITE_DONE){
			printf("Error on SQLite step 2 - %d. => %s\n",ret,seq_central);
			exit(1);
		}
		
		// Insere link entre as sequencias
		sqlite3_bind_text(stmt_insert_link,1,seq_central,-1,SQLITE_TRANSIENT);
		sqlite3_bind_text(stmt_insert_link,2,seq_cincoL,-1,SQLITE_TRANSIENT);
		
		ret = sqlite3_step(stmt_insert_link);	
		
		sqlite3_clear_bindings(stmt_insert_link);
		sqlite3_reset(stmt_insert_link);
		if(ret != SQLITE_DONE){
			printf("Error on SQLite step 2 - %d. => %s\n",ret,seq_central);
			exit(1);
		}
	}*/
    
    if(tipo == SENSO){
		// Atualiza contagem senso da seq_central
		sqlite3_bind_int(stmt_update_senso,1,hash);
		
		ret = sqlite3_step(stmt_update_senso);	
        
        sqlite3_clear_bindings(stmt_update_senso);
		sqlite3_reset(stmt_update_senso);
		if(ret != SQLITE_DONE){
			printf("Error on SQLite step 4 - %d. => %s\n",ret,seq_central);
			exit(1);
		}
		/*if(seq_cincoL != NULL){
			// Atualiza contagem senso da seq_cincol;
			sqlite3_bind_text(stmt_update_senso_cincol,1,seq_central,-1,SQLITE_TRANSIENT);
			sqlite3_bind_text(stmt_update_senso_cincol,2,seq_cincoL,-1,SQLITE_TRANSIENT);
			
			ret = sqlite3_step(stmt_update_senso_cincol);	
			
			sqlite3_clear_bindings(stmt_update_senso_cincol);
			sqlite3_reset(stmt_update_senso_cincol);
			if(ret != SQLITE_DONE){
				printf("Error on SQLite step 5 - %d. => %s\n",ret,seq_central);
				exit(1);
			}
		}*/	
	}else{
		// Atualiza contagem antisenso da seq_central
		sqlite3_bind_int(stmt_update_antisenso,1,hash);
		
		ret = sqlite3_step(stmt_update_antisenso);
		
		sqlite3_reset(stmt_update_antisenso);	
		sqlite3_clear_bindings(stmt_update_antisenso);
		if(ret != SQLITE_DONE){
			printf("Error on SQLite step 6 - %d. => %s\n",ret,seq_central);
			exit(1);
		}
		/*if(seq_cincoL != NULL){
			// Atualiza contagem senso da seq_cincol;
			sqlite3_bind_text(stmt_update_antisenso_cincol,1,seq_central,-1,SQLITE_TRANSIENT);
			sqlite3_bind_text(stmt_update_antisenso_cincol,2,seq_cincoL,-1,SQLITE_TRANSIENT);
			
			ret = sqlite3_step(stmt_update_antisenso_cincol);
			
			sqlite3_reset(stmt_update_antisenso_cincol);	
			sqlite3_clear_bindings(stmt_update_antisenso_cincol);
			if(ret != SQLITE_DONE){
				printf("Error on SQLite step 7 - %d. => %s\n",ret,seq_central);
				exit(1);
			}
		}*/
	}
	
	count++;
	return;
}

void db_destroy(){
    char * sErrMsg;
	int ret;
	char query[] = "UPDATE events SET pares = min(qnt_sensos,qnt_antisensos)";
	
	if(!destroyed){
		ret = sqlite3_exec(db,query,NULL, NULL,&sErrMsg);
		
		sqlite3_finalize(stmt_insert_main);
		sqlite3_finalize(stmt_insert);
		//sqlite3_finalize(stmt_insert_cincol);
		//sqlite3_finalize(stmt_insert_link);
		//sqlite3_finalize(stmt_select_cincol);
		sqlite3_finalize(stmt_update_antisenso);
		//sqlite3_finalize(stmt_update_antisenso_cincol);
		sqlite3_finalize(stmt_update_senso);
		//sqlite3_finalize(stmt_update_senso_cincol);
		sqlite3_close(db);
		destroyed = 1;
	}
}

int callback(void *NotUsed,int argc,char **argv,char **azColName){
	int i;
  /*for(i=0;i<argc;i++){
  	printf("%s ",azColName[i]);
  }
  printf("\n");
*/
  for(i=0; i<argc; i++){
    if( argv[i] )
        printf("%s ",argv[i]);
    else
        printf("NULL ");
  }
  printf("\n");
  return 0;

}

void db_select(char *query){
	char *db_err;
	sqlite3_exec(db,query,callback,0,&db_err);
}
