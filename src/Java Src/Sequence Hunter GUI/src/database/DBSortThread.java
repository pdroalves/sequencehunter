package database;

import gui.Drawer;

public class DBSortThread extends Thread{
	private DB db;
	private String sortQuery;
	private DBManager caller;
	private final String paresSortDesc = "SELECT sequence,qnt_sensos,qnt_antisensos,pares FROM events e inner join main_sequences ms on e.fk_main_seq = ms.id ORDER BY pares DESC";
	private final String sensosSortDesc = "SELECT sequence,qnt_sensos,qnt_antisensos,pares FROM events e inner join main_sequences ms on e.fk_main_seq = ms.id ORDER BY qnt_sensos DESC";
	private final String antisensosSortDesc = "SELECT sequence,qnt_sensos,qnt_antisensos,pares FROM events e inner join main_sequences ms on e.fk_main_seq = ms.id ORDER BY qnt_antisensos DESC";
	private final String seqSortAsc = "SELECT sequence,qnt_sensos,qnt_antisensos,pares FROM events e inner join main_sequences ms on e.fk_main_seq = ms.id ORDER BY main_seq ASC";
	private final String paresSortAsc = "SELECT sequence,qnt_sensos,qnt_antisensos,pares FROM events e inner join main_sequences ms on e.fk_main_seq = ms.id ORDER BY pares ASC";
	private final String sensosSortAsc = "SELECT sequence,qnt_sensos,qnt_antisensos,pares FROM events e inner join main_sequences ms on e.fk_main_seq = ms.id ORDER BY qnt_sensos ASC";
	private final String antisensosSortAsc = "SELECT sequence,qnt_sensos,qnt_antisensos,pares FROM events e inner join main_sequences ms on e.fk_main_seq = ms.id ORDER BY qnt_antisensos ASC";
	private final String seqSortDesc = "SELECT sequence,qnt_sensos,qnt_antisensos,pares FROM events e inner join main_sequences ms on e.fk_main_seq = ms.id ORDER BY main_seq DESC";
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
		sortQuery = defaultSort;
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
		boolean sortStatus = db.loadQuery(sortQuery);
		if(!sortStatus){
			Drawer.writeToLog("Database ERROR!");
		}else{
			caller.setDatabaseReady();
		}
	}
}
