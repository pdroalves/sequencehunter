package database;

import gui.Drawer;

public class DBSortThread extends Thread {
	private DB db;
	private String sortQuery;
	public DBSortThread(DB database){
		db = database;
		sortQuery = "SELECT main_seq,qnt_sensos,qnt_antisensos FROM events ORDER BY pares DESC";
	}

	public void run(){
		boolean sortStatus = db.loadQuery(sortQuery);
		if(!sortStatus){
			Drawer.writeToLog("Database ERROR!");
		}
	}
}