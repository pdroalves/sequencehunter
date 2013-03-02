package tables;

import hunt.Evento;
import java.util.ArrayList;
import database.DB;


public class JReportLoadData{
	private DB db;
	private String database;
 	private ArrayList<Evento> seqs;
	
	public JReportLoadData(String databaseFilename,ArrayList<Evento> e){
		this.database = databaseFilename;
		db = new DB(databaseFilename);
		seqs = e;
	}
	
	public void load(){
		db.loadQuery();
		Evento e = db.getEvento();
		while(e != null){
			seqs.add(e);
			e = db.getEvento();
		}
	}

}
