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
int destroyed;
sqlite3_stmt *stmt_insert_main;
sqlite3_stmt *stmt_insert_5l;
sqlite3_stmt *stmt_insert_full;
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
	
  // Main table
  strcpy(query,"CREATE TABLE events_tmp (main_seq TEXT,senso INTEGER DEFAULT 0,antisenso INTEGER DEFAULT 0)");
  // Execute the query for creating the table
  ret = sqlite3_exec(db,query,NULL, NULL,&sErrMsg);
  if(sErrMsg != NULL){
    printf("DB error: %s - %s\n",sErrMsg,filename);
    exit(1);
  }
  // 5l region
  strcpy(query,"CREATE TABLE events_5l_tmp (seq TEXT,senso INTEGER DEFAULT 0,antisenso INTEGER DEFAULT 0)");
  // Execute the query for creating the table
  ret = sqlite3_exec(db,query,NULL, NULL,&sErrMsg);
  if(sErrMsg != NULL){
    printf("DB error: %s - %s\n",sErrMsg,filename);
    exit(1);
  }
  strcpy(query,"CREATE TABLE events_full (seq TEXT,central_cut TEXT)");
  // Execute the query for creating the table
  ret = sqlite3_exec(db,query,NULL, NULL,&sErrMsg);
  if(sErrMsg != NULL){
    printf("DB error: %s - %s\n",sErrMsg,filename);
    exit(1);
  }
	
  // Main
  //
  // Compile insert-statement
  sprintf(query, "INSERT INTO events_tmp (main_seq,senso,antisenso) VALUES (@SEQ,@SEN,@ANT)");
  ret = sqlite3_prepare_v2(db,  query, -1, &stmt_insert_main, 0);
  if(ret != SQLITE_OK){
    printf("Error on statement compile 1b - %d.\n",ret);
    exit(1);
  }
  sprintf(query, "INSERT INTO events_5l_tmp (seq,senso,antisenso) VALUES (@SEQ,@SEN,@ANT)");
  ret = sqlite3_prepare_v2(db,  query, -1, &stmt_insert_5l, 0);
  if(ret != SQLITE_OK){
    printf("Error on statement compile 1b - %d.\n",ret);
    exit(1);
  }
  sprintf(query, "INSERT INTO events_full (seq,central_cut) VALUES (@SEQ,@CEN)");
  ret = sqlite3_prepare_v2(db,  query, -1, &stmt_insert_full, 0);
  if(ret != SQLITE_OK){
    printf("Error on statement compile 1b - %d.\n",ret);
    exit(1);
  }


	
  sqlite3_soft_heap_limit64(MAX_DB_MEM_USE*0.8);
	
  destroyed = 0;

  free(query);
  return;
}

void db_add(char *full_seq,char *seq_central,char *seq_cincoL,int tipo){
  int ret=0;
  int id;
  char * sErrMsg;
 

  if(full_seq != NULL){
    sqlite3_bind_text(stmt_insert_full,1,full_seq,-1,SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt_insert_full,2,seq_central,-1,SQLITE_TRANSIENT);
  }

  sqlite3_bind_text(stmt_insert_main,1,seq_central,-1,SQLITE_TRANSIENT);
  if(seq_cincoL != NULL){
    sqlite3_bind_text(stmt_insert_5l,1,seq_cincoL,-1,SQLITE_TRANSIENT);
  }

  if(tipo == SENSO){
    // Atualiza contagem senso da seq_central
    sqlite3_bind_int(stmt_insert_main,2,1);
    sqlite3_bind_int(stmt_insert_main,3,0);

    if(seq_cincoL != NULL){
      // Atualiza contagem senso 5l

      sqlite3_bind_int(stmt_insert_5l,2,1);
      sqlite3_bind_int(stmt_insert_5l,3,0);
    }
  }else{
    // Atualiza contagem antisenso da seq_centrall	
    sqlite3_bind_int(stmt_insert_main,2,0);
    sqlite3_bind_int(stmt_insert_main,3,1);
	
    if(seq_cincoL != NULL){
      // Atualiza contagem senso 5l
	   		
      sqlite3_bind_int(stmt_insert_5l,2,0);
      sqlite3_bind_int(stmt_insert_5l,3,1);
    }
  }
	
  if(full_seq != NULL){
    ret = sqlite3_step(stmt_insert_full);
    sqlite3_clear_bindings(stmt_insert_full);
    sqlite3_reset(stmt_insert_full);
    if(ret != SQLITE_DONE){
      printf("Error on SQLite step 4 - %d. => %s\n",ret,seq_central);
      exit(1);
    }
  }
  ret = sqlite3_step(stmt_insert_main);
  sqlite3_clear_bindings(stmt_insert_main);
  sqlite3_reset(stmt_insert_main);
  if(ret != SQLITE_DONE){
    printf("Error on SQLite step 4 - %d. => %s\n",ret,seq_central);
    exit(1);
  }

  if(seq_cincoL != NULL){
    ret = sqlite3_step(stmt_insert_5l);
    sqlite3_clear_bindings(stmt_insert_5l);
    sqlite3_reset(stmt_insert_5l);
    if(ret != SQLITE_DONE){
      printf("Error on SQLite step 4 - %d. => %s\n",ret,seq_cincoL);
      exit(1);
    }
  }


  return;
}

void db_destroy(){
  char *sErrMsgMain,*sErrMsg5l,*sErrMsgFull,*sErrMsg;
  int ret;
  char createEventsQuery[] = "CREATE TABLE IF NOT EXISTS events as SELECT main_seq,SUM(senso) qnt_sensos,SUM(antisenso) qnt_antisensos,min(SUM(senso),SUM(antisenso)) pares FROM events_tmp GROUP BY main_seq";
  char create5lEventsQuery[] = "CREATE TABLE IF NOT EXISTS events_5l as SELECT seq,SUM(senso) qnt_sensos,SUM(antisenso) qnt_antisensos,min(SUM(senso),SUM(antisenso)) pares FROM events_5l_tmp GROUP BY seq";
  char dropTmpQuery[] = "DROP TABLE events_tmp";
  char drop5lTmpQuery[] = "DROP TABLE events_5l_tmp";
  int errorMain,error5l,errorFull;
	
  if(!destroyed){
    // Create tables
    db_start_transaction();
    errorMain = sqlite3_exec(db,createEventsQuery,NULL, NULL,&sErrMsgMain);
    error5l = sqlite3_exec(db,create5lEventsQuery,NULL, NULL,&sErrMsg5l);
    db_commit_transaction();

    if(sErrMsg5l == NULL && sErrMsgMain == NULL && sErrMsgFull == NULL){
      // Drop tables
      db_start_transaction();
      sqlite3_exec(db,dropTmpQuery,NULL, NULL,&sErrMsgMain);
      sqlite3_exec(db,drop5lTmpQuery,NULL, NULL,&sErrMsg5l);
      db_commit_transaction();

      // Clean
      sqlite3_exec(db,"vacuum",NULL, NULL,&sErrMsg);	
    }else{
      if(errorMain == SQLITE_FULL){
	printf("Database ERROR! %s\nPlease, free up some hard disk space and run Sequence Hunter again passing '%s --fixdb' as parameter.\n",sErrMsgMain,database_path);
      }else{
	if(error5l == SQLITE_FULL){
	  printf("Database ERROR! %s\nPlease, free up some hard disk space and run Sequence Hunter again passing '%s --fixdb' as parameter.\n",sErrMsg5l,database_path);
	}else{
	    printf("Database ERROR! \nFull: %s\nMain: %s\n5l: %s\n",sErrMsgFull,sErrMsgMain,sErrMsg5l);
	}
      }
    }
    sqlite3_finalize(stmt_insert_main);
    sqlite3_finalize(stmt_insert_5l);
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
  int errorMain,error5l,errorFull;
  char *sErrMsgMain,*sErrMsg5l,*sErrMsgFull,*sErrMsg;
  char createEventsQuery[] = "CREATE TABLE IF NOT EXISTS events as SELECT main_seq,SUM(senso) qnt_sensos,SUM(antisenso) qnt_antisensos,min(SUM(senso),SUM(antisenso)) pares FROM events_tmp GROUP BY main_seq";
  char create5lEventsQuery[] = "CREATE TABLE IF NOT EXISTS events_5l as SELECT seq,SUM(senso) qnt_sensos,SUM(antisenso) qnt_antisensos,min(SUM(senso),SUM(antisenso)) pares FROM events_5l_tmp GROUP BY seq";
  char dropTmpQuery[] = "DROP TABLE events_tmp";
  char drop5lTmpQuery[] = "DROP TABLE events_5l_tmp";
  
  sqlite3_open(filename,&db);
    
  if (!db){
    printf("Not sure why, but the database didn't open.\n");
    exit(1);
  }
  // Create tables
  db_start_transaction();
  errorMain = sqlite3_exec(db,createEventsQuery,NULL, NULL,&sErrMsgMain);
  error5l = sqlite3_exec(db,create5lEventsQuery,NULL, NULL,&sErrMsg5l);
  db_commit_transaction();
  if(sErrMsg5l == NULL && sErrMsgMain == NULL && sErrMsgMain){
    // Drop tables
    db_start_transaction();
    errorMain = sqlite3_exec(db,createEventsQuery,NULL, NULL,&sErrMsgMain);
    error5l = sqlite3_exec(db,create5lEventsQuery,NULL, NULL,&sErrMsg5l);
    db_commit_transaction();
			
    sqlite3_exec(db,"vacuum",NULL, NULL,&sErrMsg);	
  }else{
    if(errorMain == SQLITE_FULL){
      printf("Database ERROR! %s\nPlease, free up some hard disk space and run Sequence Hunter again passing '%s --fixdb' as parameter.\n",sErrMsgMain,database_path);
    }else{
      if(error5l == SQLITE_FULL){
	printf("Database ERROR! %s\nPlease, free up some hard disk space and run Sequence Hunter again passing '%s --fixdb' as parameter.\n",sErrMsg5l,database_path);
      }else{
	  printf("Database ERROR! \nMain: %s\n5l: %s\n",sErrMsgMain,sErrMsg5l);
	    }
    }
  }
  sqlite3_close(db);
  return;
}
