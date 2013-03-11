package tables;

import gui.Drawer;
import hunt.Evento;
import java.util.ArrayList;
import database.DB;
import database.DBManager;


public class JReportLoadData{
	private DBManager dbm;
	private String database;
	private int defaultLoad;
	private ArrayList<Evento> seqs;

	public JReportLoadData(String databaseFilename,int dl,ArrayList<Evento> al){
		this.database = databaseFilename;
		dbm = new DBManager();
		dbm.openDatabase(databaseFilename);
		Drawer.writeToLog("Sorting data. Please wait...");
		dbm.loadSorted();
		Drawer.writeToLog("Data sorted...");
		seqs = al;
		defaultLoad = dl;

		for(int i = 0; i < defaultLoad;i++){
			Evento e = dbm.getEvento();
			if(e != null){
				seqs.add(e);
			}else{
				return;
			}
		}
	}

	public void load(){
		Evento e = dbm.getEvento();
		if(e != null) 
			seqs.add(e);
		else 
			return;
	}

}
