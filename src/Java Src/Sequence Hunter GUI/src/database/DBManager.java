package database;

import hunt.Evento;

public class DBManager extends Thread{
	private DB database;
	private DBSortThread dbst;

	public DBManager(){

	}

	public boolean openDatabase(String databaseFilename){
		database = new DB(databaseFilename);
		if(database != null){
			dbst = new DBSortThread(database);
			return true;
		}else{
			return false;
		}
	}

	public void loadSorted(){
		dbst.go();
	}

	public Evento getEvento(){
		return database.getEvento();
	}

}
