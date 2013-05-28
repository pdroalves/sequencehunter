package database;

import gui.Drawer;

public class DBSortThread extends Thread{
	private DB db;
	private String sortQuery;
	private DBManager caller;
	private final String paresSortDesc = "SELECT main_seq,qnt_sensos,qnt_antisensos,pares FROM events ORDER BY pares DESC";
	private final String sensosSortDesc = "SELECT main_seq,qnt_sensos,qnt_antisensos,pares FROM events ORDER BY qnt_sensos DESC";
	private final String antisensosSortDesc = "SELECT main_seq,qnt_sensos,qnt_antisensos,pares FROM events ORDER BY qnt_antisensos DESC";
	private final String seqSortAsc = "SELECT main_seq,qnt_sensos,qnt_antisensos,pares FROM events ORDER BY main_seq ASC";
	private final String paresSortAsc = "SELECT main_seq,qnt_sensos,qnt_antisensos,pares FROM events ORDER BY pares ASC";
	private final String sensosSortAsc = "SELECT main_seq,qnt_sensos,qnt_antisensos,pares FROM events ORDER BY qnt_sensos ASC";
	private final String antisensosSortAsc = "SELECT main_seq,qnt_sensos,qnt_antisensos,pares FROM events ORDER BY qnt_antisensos ASC";
	private final String seqSortDesc = "SELECT main_seq,qnt_sensos,qnt_antisensos,pares FROM events ORDER BY main_seq DESC";
	private final String defaultSort = paresSortDesc;
	public final int PARES_DESC = 0;
	public final int SENSOS_DESC = 1;
	public final int ANTISENSOS_DESC = 2;
	public final int SEQ_DESC = 3;
	public final int PARES_ASC = 4;
	public final int SENSOS_ASC = 5;
	public final int ANTISENSOS_ASC = 6;
	public final int SEQ_ASC = 7;

	public DBSortThread(DBManager caller,DB database){
		this.caller = caller;
		db = database;
		setSortMode(PARES_DESC);
	}

	public void setSortMode(int mode){
		switch(mode){
		case PARES_DESC:
			sortQuery = paresSortDesc;
			break;
		case SENSOS_DESC:
			sortQuery = sensosSortDesc;
			break;
		case ANTISENSOS_DESC:
			sortQuery = antisensosSortDesc;
			break;
		case SEQ_DESC:
			sortQuery = seqSortDesc;
			break;
		case PARES_ASC:
			sortQuery = paresSortAsc;
			break;
		case SENSOS_ASC:
			sortQuery = sensosSortAsc;
			break;
		case ANTISENSOS_ASC:
			sortQuery = antisensosSortAsc;
			break;
		case SEQ_ASC:
			sortQuery = seqSortAsc;
			break;
		}
	}

	public void run(){
		System.err.println("Table sort! "+sortQuery);
		boolean sortStatus = db.loadQuery(sortQuery+" limit 500");
		if(!sortStatus){
			Drawer.writeToLog("Database ERROR!");
		}else{
			caller.setDatabaseReady();
		}
	}
}
