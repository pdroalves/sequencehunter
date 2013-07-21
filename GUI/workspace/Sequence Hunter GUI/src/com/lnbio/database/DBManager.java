package com.lnbio.database;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.Observable;

import com.lnbio.gui.Drawer;
import com.lnbio.hunt.Evento;

public class DBManager extends Observable{
	private DB database;
	private boolean centralCutReady;
	private boolean fiveCutReady;
	private ArrayList<Evento> seqsCentralCut;
	private ArrayList<Evento> seqsFiveCut;
	protected int defaultLoad = 100;
	private int mode = Evento.VALUE_PARES_REL;
	static public final int DESC = 0;
	static public final int ASC = 1;
	private int totalCentralCutPares;
	private int totalCentralCutSensos;
	private int totalCentralCutAntisensos;
	private int totalCentralCutSequences;
	private int totalFiveCutSequences;
	private boolean fiveCutSupported;
	private int totalFiveCutAntisensos;
	private int totalFiveCutSensos;
	private int totalFiveCutPares;

	public DBManager(String databaseFilename){
		super();
		setCentralCutReady(false);
		setFiveCutReady(false);
		database = new DB(databaseFilename);
		totalFiveCutSequences = database.getFiveCutSize();
		totalCentralCutSequences = database.getCentralCutSize();
		seqsCentralCut = new ArrayList<Evento>(totalCentralCutSequences);
		seqsFiveCut = new ArrayList<Evento>(totalFiveCutSequences);
		totalCentralCutPares = this.getTotalCentralCutPares();
		totalCentralCutSensos = this.getTotalCentralCutSensos();
		totalCentralCutAntisensos = this.getTotalCentralCutAntisensos();
		totalFiveCutPares = this.getTotalFiveCutPares();
		totalFiveCutSensos = this.getTotalFiveCutSensos();
		totalFiveCutAntisensos = this.getTotalFiveCutAntisensos();
		if(totalFiveCutSequences > 0)
			setFiveCutSupported(true);
		if(database != null){
			this.sortCentralCut(getMode(), 0);
			this.sortFiveCut(getMode(), 0);
		}
	}

	public void sortCentralCut(int column,int ordem){
		setCentralCutReady(false);
		seqsCentralCut.clear();
		DBSortThread dbstCentralCut = new DBSortThread(this,database,DBSortThread.CENTRAL_CUT_SORT);		
		
		if(ordem == DBManager.DESC){
			switch(column){
			case 3:
				this.setMode(Evento.VALUE_SENSOS_REL);
				dbstCentralCut.setSortMode(dbstCentralCut.SENSOS_DESC);
				break;
			case 4:
				this.setMode(Evento.VALUE_ANTISENSO_REL);
				dbstCentralCut.setSortMode(dbstCentralCut.ANTISENSOS_DESC);
				break;
			default:
				this.setMode(Evento.VALUE_PARES_REL);
				dbstCentralCut.setSortMode(dbstCentralCut.PARES_DESC);
				break;
			}
		}else{
			switch(column){
			case 3:
				this.setMode(Evento.VALUE_SENSOS_REL);
				dbstCentralCut.setSortMode(dbstCentralCut.SENSOS_ASC);
				break;
			case 4:
				this.setMode(Evento.VALUE_ANTISENSO_REL);
				dbstCentralCut.setSortMode(dbstCentralCut.ANTISENSOS_ASC);
				break;
			default:
				this.setMode(Evento.VALUE_PARES_REL);
				dbstCentralCut.setSortMode(dbstCentralCut.PARES_ASC);
				break;
			}
		}
		dbstCentralCut.start();
	}

	public void sortFiveCut(int column,int ordem){
		setFiveCutReady(false);
		seqsFiveCut.clear();
		DBSortThread dbstFiveCut = new DBSortThread(this,database,DBSortThread.FIVE_CUT_SORT);		
		
		if(ordem == DBManager.DESC){
			switch(column){
			case 3:
				this.setMode(Evento.VALUE_SENSOS_REL);
				dbstFiveCut.setSortMode(dbstFiveCut.SENSOS_DESC);
				break;
			case 4:
				this.setMode(Evento.VALUE_ANTISENSO_REL);
				dbstFiveCut.setSortMode(dbstFiveCut.ANTISENSOS_DESC);
				break;
			default:
				this.setMode(Evento.VALUE_PARES_REL);
				dbstFiveCut.setSortMode(dbstFiveCut.PARES_DESC);
				break;
			}
		}else{
			switch(column){
			case 3:
				this.setMode(Evento.VALUE_SENSOS_REL);
				dbstFiveCut.setSortMode(dbstFiveCut.SENSOS_ASC);
				break;
			case 4:
				this.setMode(Evento.VALUE_ANTISENSO_REL);
				dbstFiveCut.setSortMode(dbstFiveCut.ANTISENSOS_ASC);
				break;
			default:
				this.setMode(Evento.VALUE_PARES_REL);
				dbstFiveCut.setSortMode(dbstFiveCut.PARES_ASC);
				break;
			}
		}
		dbstFiveCut.start();
	}
	
	public void setCentralCutDatabaseReady(){
		this.startCentralCutLoad();
		this.setCentralCutReady(true);
	}
	
	public void setFiveCutDatabaseReady(){
		this.startFiveCutLoad();
		this.setFiveCutReady(true);
	}

	public boolean isReady() {
		return centralCutReady;
	}

	private void setCentralCutReady(boolean ready) {
		if(ready)
			System.out.println("Seqs size: "+seqsCentralCut.size());

		this.centralCutReady = ready;
		// Avisa os observadores da mudanca
		System.err.println("DBM: Update para "+super.countObservers()+" observadores");
		super.setChanged();
		super.notifyObservers(this);
	}
	
	private void setFiveCutReady(boolean ready) {
		if(ready)
			System.out.println("Seqs size: "+seqsFiveCut.size());

		this.fiveCutReady = ready;
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
			norma = totalCentralCutSensos;
			for(int i = 0;i < seqsCentralCut.size();i++){
				Evento g = (Evento)seqsCentralCut.get(i);
				g.setRelativeFreq(g.getSensos()*100 / norma);
				g.setMode(mode);
			}
			break;
		case Evento.VALUE_ANTISENSO_REL:
			norma = totalCentralCutAntisensos;
			for(int i = 0;i < seqsCentralCut.size();i++){
				Evento g = (Evento)seqsCentralCut.get(i);
				g.setRelativeFreq(g.getAntisensos()*100 / norma);
				g.setMode(mode);
			}
			break;
		default:
			norma = totalCentralCutPares;
			for(int i = 0;i < seqsCentralCut.size();i++){
				Evento g = (Evento)seqsCentralCut.get(i);
				g.setRelativeFreq(g.getPares()*100 / norma);
				g.setMode(mode);
			}
			break;
		}		
	}

	public ArrayList<Evento> getCentralCutEvents() {
		return seqsCentralCut;
	}

	public DB getDB(){
		return database;
	}

	public Evento getCentralCutEvento(){
		return database.getCentralCutEvento();
	}
	
	public Evento getFiveCutEvento(){
		return database.getFiveCutEvento();
	}

	public void startCentralCutLoad(){
		for(int i = 0; i < defaultLoad;i++){
			Evento e = this.getCentralCutEvento();
			if(e != null){
				seqsCentralCut.add(e);
			}else{
				return;
			}
		}
	}

	public void startFiveCutLoad(){
		for(int i = 0; i < defaultLoad;i++){
			Evento e = this.getFiveCutEvento();
			if(e != null){
				seqsFiveCut.add(e);
			}else{
				return;
			}
		}
	}

	public void centralCutLoad(){
		Evento e = this.getCentralCutEvento();
		if(e != null) 
			seqsCentralCut.add(e);
		else 
			return;
	}
	
	public void fiveCutLoad(){
		Evento e = this.getFiveCutEvento();
		if(e != null) 
			seqsFiveCut.add(e);
		else 
			return;
	}

	public void centralCutLoad(long quantity){
		// Carrega sequencias ate atingir o tamanho de quantity 
		// ou ate nao haverem mais sequencias
		Evento e = this.getCentralCutEvento();

		while(e != null && seqsCentralCut.size() < quantity) {
			seqsCentralCut.add(e);
			e = this.getCentralCutEvento();
		}
	}

	public void destroy(){
		seqsCentralCut.clear();
		seqsFiveCut.clear();
		database.close();
	}

	public int getTotalCentralCutPares(){
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

	public int getTotalCentralCutSensos(){
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

	public int getTotalCentralCutAntisensos(){
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

	public int getTotalFiveCutPares(){
		int total = 0;
		ResultSet rs = database.executeQuery("SELECT SUM(pares) FROM events_5l");
		try {
			if(rs.next())
				total = rs.getInt(1);
		} catch (SQLException e) {
			Drawer.writeToLog("Database error!");
		}
		return total;
	}

	public int getTotalFiveCutSensos(){
		int total = 0;
		ResultSet rs = database.executeQuery("SELECT SUM(qnt_sensos) FROM events_5l");
		try {
			if(rs.next())
				total = rs.getInt(1);
		} catch (SQLException e) {
			Drawer.writeToLog("Database error!");
		}
		return total;
	}

	public int getTotalFiveCutAntisensos(){
		int total = 0;
		ResultSet rs = database.executeQuery("SELECT SUM(qnt_antisensos) FROM events_5l");
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

	public boolean isFiveCutSupported() {
		return fiveCutSupported;
	}

	public void setFiveCutSupported(boolean supportCincoL) {
		this.fiveCutSupported = supportCincoL;
	}
}
