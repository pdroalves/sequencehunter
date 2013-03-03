package tables;

import hunt.Evento;
import java.util.ArrayList;
import database.DB;


public class JReportLoadData{
	private DB db;
	private String database;
	
	public JReportLoadData(String databaseFilename){
		this.database = databaseFilename;
		db = new DB(databaseFilename);
		db.loadQuery();
	}
	
	public Evento load(){
		return db.getEvento();
	}

}
