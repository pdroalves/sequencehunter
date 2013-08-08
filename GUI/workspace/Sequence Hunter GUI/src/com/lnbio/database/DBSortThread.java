package com.lnbio.database;

import com.lnbio.gui.Drawer;

public class DBSortThread extends Thread{
	private DB db;
	private DBManager caller;
	private String centralCutSortQuery;
	private String fiveCutSortQuery;
	private final String paresSortDescCC = "SELECT main_seq,qnt_sensos,qnt_antisensos,MIN(qnt_sensos,qnt_antisensos) pares FROM events ORDER BY pares DESC";
	private final String sensosSortDescCC = "SELECT main_seq,qnt_sensos,qnt_antisensos FROM events ORDER BY qnt_sensos DESC";
	private final String antisensosSortDescCC = "SELECT main_seq,qnt_sensos,qnt_antisensos FROM events ORDER BY qnt_antisensos DESC";
	private final String seqSortAscCC = "SELECT main_seq,qnt_sensos,qnt_antisensos FROM events ORDER BY main_seq ASC";
	private final String paresSortAscCC = "SELECT main_seq,qnt_sensos,qnt_antisensos,MIN(qnt_sensos,qnt_antisensos) pares FROM events ORDER BY pares ASC";
	private final String sensosSortAscCC = "SELECT main_seq,qnt_sensos,qnt_antisensos FROM events ORDER BY qnt_sensos ASC";
	private final String antisensosSortAscCC = "SELECT main_seq,qnt_sensos,qnt_antisensos FROM events ORDER BY qnt_antisensos ASC";
	private final String seqSortDescCC = "SELECT main_seq,qnt_sensos,qnt_antisensos FROM events ORDER BY main_seq DESC";

	private final String paresSortDescFC = "SELECT seq,qnt_sensos,qnt_antisensos,MIN(qnt_sensos,qnt_antisensos) pares FROM events_5l ORDER BY pares DESC";
	private final String sensosSortDescFC = "SELECT seq,qnt_sensos,qnt_antisensos FROM events_5l ORDER BY qnt_sensos DESC";
	private final String antisensosSortDescFC = "SELECT seq,qnt_sensos,qnt_antisensos FROM events_5l ORDER BY qnt_antisensos DESC";
	private final String seqSortAscFC = "SELECT seq,qnt_sensos,qnt_antisensos FROM events_5l ORDER BY seq ASC";
	private final String paresSortAscFC = "SELECT seq,qnt_sensos,qnt_antisensos,MIN(qnt_sensos,qnt_antisensos) pares FROM events_5l ORDER BY pares ASC";
	private final String sensosSortAscFC = "SELECT seq,qnt_sensos,qnt_antisensos FROM events_5l ORDER BY qnt_sensos ASC";
	private final String antisensosSortAscFC = "SELECT seq,qnt_sensos,qnt_antisensos FROM events_5l ORDER BY qnt_antisensos ASC";
	private final String seqSortDescFC = "SELECT seq,qnt_sensos,qnt_antisensos FROM events_5l ORDER BY seq DESC";


	//private final String defaultSort = paresSortDesc;
	public final int PARES_DESC = 0;
	public final int SENSOS_DESC = 1;
	public final int ANTISENSOS_DESC = 2;
	public final int SEQ_DESC = 3;
	public final int PARES_ASC = 4;
	public final int SENSOS_ASC = 5;
	public final int ANTISENSOS_ASC = 6;
	public final int SEQ_ASC = 7;
	public static final int CENTRAL_CUT_SORT = 1;
	public static final int FIVE_CUT_SORT = 2;
	private int sortKind;

	public DBSortThread(DBManager caller,DB database,int sortKind){
		this.caller = caller;
		db = database;
		this.sortKind = sortKind;
		setSortMode(PARES_DESC);
	}

	public void setSortMode(int mode){
		if(sortKind == CENTRAL_CUT_SORT)
			switch(mode){
			case PARES_DESC:
				centralCutSortQuery = paresSortDescCC;
				break;
			case SENSOS_DESC:
				centralCutSortQuery = sensosSortDescCC;
				break;
			case ANTISENSOS_DESC:
				centralCutSortQuery = antisensosSortDescCC;
				break;
			case SEQ_DESC:
				centralCutSortQuery = seqSortDescCC;
				break;
			case PARES_ASC:
				centralCutSortQuery = paresSortAscCC;
				break;
			case SENSOS_ASC:
				centralCutSortQuery = sensosSortAscCC;
				break;
			case ANTISENSOS_ASC:
				centralCutSortQuery = antisensosSortAscCC;
				break;
			case SEQ_ASC:
				centralCutSortQuery = seqSortAscCC;
				break;
			}
		if(sortKind == FIVE_CUT_SORT)
			switch(mode){
			case PARES_DESC:
				fiveCutSortQuery = paresSortDescFC;
				break;
			case SENSOS_DESC:
				fiveCutSortQuery = sensosSortDescFC;
				break;
			case ANTISENSOS_DESC:
				fiveCutSortQuery = antisensosSortDescFC;
				break;
			case SEQ_DESC:
				fiveCutSortQuery = seqSortDescFC;
				break;
			case PARES_ASC:
				fiveCutSortQuery = paresSortAscFC;
				break;
			case SENSOS_ASC:
				fiveCutSortQuery = sensosSortAscFC;
				break;
			case ANTISENSOS_ASC:
				fiveCutSortQuery = antisensosSortAscFC;
				break;
			case SEQ_ASC:
				fiveCutSortQuery = seqSortAscFC;
				break;
			}
	}

	public void run(){
		if(sortKind == CENTRAL_CUT_SORT){
			System.err.println("Table sort! "+centralCutSortQuery);
			boolean sortStatus = db.loadCentralCutQuery(centralCutSortQuery);
			if(!sortStatus){
				Drawer.writeToLog("Database ERROR!");
			}else{
				caller.setCentralCutDatabaseReady();
			}
		}else if(sortKind == FIVE_CUT_SORT){
			System.err.println("Table sort! "+fiveCutSortQuery);
			boolean sortStatus = db.loadFiveCutQuery(fiveCutSortQuery);
			if(!sortStatus){
				Drawer.writeToLog("Database ERROR!");
			}else{
				caller.setFiveCutDatabaseReady();
			}
		}
	}
}
