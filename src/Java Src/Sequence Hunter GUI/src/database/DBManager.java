package database;

import java.util.Observable;

import hunt.Evento;

public class DBManager extends Observable{
	private DB database;
	private DBSortThread dbst;
	private boolean ready;
	
	public DBManager(String databaseFilename){
		setReady(false);
		database = new DB(databaseFilename);
		if(database != null){
			dbst = new DBSortThread(this,database);
		}
	}

	public void loadSort(){
		dbst.start();
	}
	
	public void setLoadSorted(){
		setReady(true);
	}

	public Evento getEvento(){
		return database.getEvento();
	}

	public boolean isReady() {
		return getReady() == true;
	}

	private boolean getReady(){
		return ready;
	}
	
	private void setReady(boolean ready) {
		this.ready = ready;
		// Avisa os observadores da mudanca
		super.setChanged();
		super.notifyObservers();
	}
}
