package database;

import java.util.ArrayList;
import java.util.Observable;

import hunt.Evento;

public class DBManager extends Observable{
	private DB database;
	private boolean ready;
	private ArrayList<Evento> seqs;
	protected int defaultLoad = 100;

	public DBManager(String databaseFilename){
		setReady(false);
		database = new DB(databaseFilename);
		int size = database.getSize();
		seqs = new ArrayList<Evento>(size) ;
		if(database != null){
			DBSortThread dbst = new DBSortThread(this,database);
			dbst.start();
		}
	}

	public void sort(int column,int ordem){
		setReady(false);
		seqs.clear();
		DBSortThread dbst = new DBSortThread(this,database);
		if(ordem == 0){
			switch(column){
			case 0:
				dbst.setSortMode(dbst.PARES_DESC);
				break;
			case 1:
				dbst.setSortMode(dbst.SEQ_DESC);
				break;
			case 2:
				dbst.setSortMode(dbst.PARES_DESC);
				break;
			case 3:
				dbst.setSortMode(dbst.SENSOS_DESC);
				break;
			case 4:
				dbst.setSortMode(dbst.ANTISENSOS_DESC);
				break;
			}
		}else{
			switch(column){
			case 0:
				dbst.setSortMode(dbst.PARES_ASC);
				break;
			case 1:
				dbst.setSortMode(dbst.SEQ_ASC);
				break;
			case 2:
				dbst.setSortMode(dbst.PARES_ASC);
				break;
			case 3:
				dbst.setSortMode(dbst.SENSOS_ASC);
				break;
			case 4:
				dbst.setSortMode(dbst.ANTISENSOS_ASC);
				break;
			}
		}
		dbst.start();
	}

	public void setDatabaseReady(){
		setReady(true);
	}

	public boolean isReady() {
		return ready;
	}

	private void setReady(boolean ready) {
		this.ready = ready;
		// Avisa os observadores da mudanca
		super.setChanged();
		super.notifyObservers();
		System.out.println("Update!");
	}

	public ArrayList<Evento> getEvents() {
		return seqs;
	}

	public DB getDB(){
		return database;
	}

	public Evento getEvento(){
		return database.getEvento();
	}

	public void startLoad(){
		for(int i = 0; i < defaultLoad;i++){
			Evento e = this.getEvento();
			if(e != null){
				seqs.add(e);
			}else{
				return;
			}
		}
	}

	public void load(){
		Evento e = this.getEvento();
		if(e != null) 
			seqs.add(e);
		else 
			return;
	}
}
