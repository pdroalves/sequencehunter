package database;

import java.util.ArrayList;
import java.util.Observable;

import hunt.Evento;

public class DBManager extends Observable{
	private DB database;
	private DBSortThread dbst;
	private boolean ready;
	private ArrayList<Evento> seqs;
	protected int defaultLoad = 100;
	
	public DBManager(String databaseFilename){
		setReady(false);
		database = new DB(databaseFilename);
		seqs = new ArrayList<Evento>() ;
		if(database != null){
			dbst = new DBSortThread(this,database);
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
