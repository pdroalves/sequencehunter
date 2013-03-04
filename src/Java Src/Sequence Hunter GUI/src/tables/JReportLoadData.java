package tables;

import hunt.Evento;
import java.util.ArrayList;
import database.DB;


public class JReportLoadData{
	private DB db;
	private String database;
	private int defaultLoad;
	private ArrayList<Evento> seqs;
	
	public JReportLoadData(String databaseFilename,int dl,ArrayList<Evento> al){
		this.database = databaseFilename;
		db = new DB(databaseFilename);
		db.loadQuery();
		seqs = al;
		defaultLoad = dl;

		for(int i = 0; i < defaultLoad;i++){
			Evento e = db.getEvento();
			if(e != null){
				seqs.add(e);
			}else{
				return;
			}
		}
	}
	
	public void load(){
			Evento e = db.getEvento();
			if(e != null) 
				seqs.add(e);
			else 
				return;
	}

}
