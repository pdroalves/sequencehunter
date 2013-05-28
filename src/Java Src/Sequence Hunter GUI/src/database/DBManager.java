package database;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.Observable;

import myTypeData.GenType;

import gui.Drawer;
import hunt.Evento;

public class DBManager extends Observable{
	private DB database;
	private boolean ready;
	private ArrayList<Evento> seqs;
	protected int defaultLoad = 100;
	private int[] mode = {Evento.VALUE_PARES_REL,0};

	public DBManager(String databaseFilename){
		super();
		setReady(false);
		database = new DB(databaseFilename);
		int size = database.getSize();
		seqs = new ArrayList<Evento>(size) ;
		if(database != null){
			this.sort(getMode()[0], getMode()[1]);
		}
	}

	public void sort(int column,int ordem){
		setReady(false);
		seqs.clear();
		DBSortThread dbst = new DBSortThread(this,database);
		getMode()[0] = column;
		getMode()[1] = ordem;
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
		this.startLoad();
		this.setReady(true);
	}

	public boolean isReady() {
		return ready;
	}

	private void setReady(boolean ready) {
		if(ready == true)
			System.out.println("Seqs size: "+seqs.size());
		
		this.ready = ready;
		// Avisa os observadores da mudanca
		System.err.println("DBM: Update para "+super.countObservers()+" observadores");
		super.setChanged();
		super.notifyObservers(this);
	}
	public void normalizeData(int norma,int mode){
		for(int i = 0;i < seqs.size();i++){
			Evento g = (Evento)seqs.get(i);
			g.setRelativeFreq(g.getValue()*100 / (float)norma);
			g.setMode(mode);
			System.out.println(g.getValue());
		}
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

	public void destroy(){
		seqs.clear();
		database.close();
	}

	public int getTotalPares(){
		ResultSet rs = database.executeQuery("SELECT SUM(pares) FROM events");
		int total = 0;
		try {
			if(rs.next())
				total = rs.getInt(1);
		} catch (SQLException e) {
			Drawer.writeToLog("Database error! - "+e.getMessage());
		}
		return total;
	}

	public int[] getMode() {
		return mode;
	}

	public void setMode(int[] mode) {
		this.mode = mode;
	}
}
