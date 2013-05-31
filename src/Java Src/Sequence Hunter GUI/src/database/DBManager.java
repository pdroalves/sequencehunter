package database;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.Observable;

import xml.TranslationsManager;

import gui.Drawer;
import hunt.Evento;

public class DBManager extends Observable{
	private DB database;
	private boolean ready;
	private ArrayList<Evento> seqs;
	protected int defaultLoad = 100;
	private int mode = Evento.VALUE_PARES_REL;
	static public final int DESC = 0;
	static public final int ASC = 1;
	public int totalPares;
	public int totalSensos;
	public int totalAntisensos;
	public int totalSequences;

	public DBManager(String databaseFilename){
		super();
		setReady(false);
		database = new DB(databaseFilename);
		int size = database.getSize();
		seqs = new ArrayList<Evento>(size) ;
		totalPares = this.getTotalPares();
		totalSensos = this.getTotalSensos();
		totalAntisensos = this.getTotalAntisensos();
		totalSequences = database.getSize();
		if(database != null){
			this.sort(getMode(), 0);
		}
	}

	public void sort(int column,int ordem){
		setReady(false);
		seqs.clear();
		DBSortThread dbst = new DBSortThread(this,database);
		if(ordem == this.DESC){
			switch(column){
			case 3:
				this.setMode(Evento.VALUE_SENSOS_REL);
				dbst.setSortMode(dbst.SENSOS_DESC);
				break;
			case 4:
				this.setMode(Evento.VALUE_ANTISENSO_REL);
				dbst.setSortMode(dbst.ANTISENSOS_DESC);
				break;
			default:
				this.setMode(Evento.VALUE_PARES_REL);
				dbst.setSortMode(dbst.PARES_DESC);
				break;
			}
		}else{
			switch(column){
			case 3:
				this.setMode(Evento.VALUE_SENSOS_REL);
				dbst.setSortMode(dbst.SENSOS_ASC);
				break;
			case 4:
				this.setMode(Evento.VALUE_ANTISENSO_REL);
				dbst.setSortMode(dbst.ANTISENSOS_ASC);
				break;
			default:
				this.setMode(Evento.VALUE_PARES_REL);
				dbst.setSortMode(dbst.PARES_ASC);
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
	public void normalizeData(){
		System.out.println("Normalizando");
		float norma;
		switch(mode){
		case Evento.VALUE_SENSOS_REL:
			norma = totalSensos;
			for(int i = 0;i < seqs.size();i++){
				Evento g = (Evento)seqs.get(i);
				g.setRelativeFreq(g.getSensos()*100 / norma);
				g.setMode(mode);
			}
			break;
		case Evento.VALUE_ANTISENSO_REL:
			norma = totalAntisensos;
			for(int i = 0;i < seqs.size();i++){
				Evento g = (Evento)seqs.get(i);
				g.setRelativeFreq(g.getAntisensos()*100 / norma);
				g.setMode(mode);
			}
			break;
		default:
			norma = totalPares;
			for(int i = 0;i < seqs.size();i++){
				Evento g = (Evento)seqs.get(i);
				g.setRelativeFreq(g.getPares()*100 / norma);
				g.setMode(mode);
			}
			break;
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
		int total = 0;
		ResultSet rs = database.executeQuery("SELECT SUM(pares) FROM events");
		try {
			if(rs.next())
				total = rs.getInt(1);
		} catch (SQLException e) {
			Drawer.writeToLog("Database error!");
		}
		return total;
	}

	public int getTotalSensos(){
		int total = 0;
		ResultSet rs = database.executeQuery("SELECT SUM(qnt_sensos) FROM events");
		try {
			if(rs.next())
				total = rs.getInt(1);
		} catch (SQLException e) {
			Drawer.writeToLog("Database error!");
		}
		return total;
	}

	public int getTotalAntisensos(){
		int total = 0;
		ResultSet rs = database.executeQuery("SELECT SUM(qnt_antisensos) FROM events");
		try {
			if(rs.next())
				total = rs.getInt(1);
		} catch (SQLException e) {
			Drawer.writeToLog("Database error!");
		}
		return total;
	}

	public int getMode() {
		return mode;
	}

	public void setMode(int tipo) {
		this.mode = tipo;
	}
}
