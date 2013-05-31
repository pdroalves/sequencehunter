package database;

import gui.Drawer;
import hunt.Evento;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;

import xml.TranslationsManager;

public class DB {
	private Connection databaseConn;
	private ResultSet rows;
	private final int maxRepeats = 3;

	public DB(String databaseFilename){
		boolean repeat = true;
		int repeats = 0;
		while(repeat == true && repeats < maxRepeats){	
			try {
				// Carrega dinamicamente a lib
				Class.forName("org.sqlite.JDBC");
				databaseConn = DriverManager.getConnection("jdbc:sqlite:"+databaseFilename);
			} catch (ClassNotFoundException e) {
				Drawer.writeToLog("Database ERROR: "+e.getMessage());
			} catch (SQLException e) {
				Drawer.writeToLog("Database ERROR: "+e.getMessage());
			}
			repeats++;
		}
	}

	public boolean loadQuery(String query){
		boolean repeat = true;
		int repeats = 0;
		Statement stat = null;
		while(repeat == true && repeats < maxRepeats){
			try {
				stat = databaseConn.createStatement();
				rows = stat.executeQuery(query);
				if(rows != null) 
					return true;
			} catch (SQLException e) {
				Drawer.writeToLog("Database ERROR on loadQuery: "+e.getMessage());
				if(e.getMessage().contains("full")){
					Drawer.writeToLog(TranslationsManager.getInstance().getText("DiskFullForTmpFile"));
					Drawer.writeToLog(TranslationsManager.getInstance().getText("LowDiskSpaceMode"));
					query.concat(" limit 100");
				}else if(e.getMessage().contains("no such table")){
					fixDatabase();
				}
			}
			repeats++;
		}
		return false;
	}

	private void fixDatabase(){
		try{
			Drawer.writeToLog(TranslationsManager.getInstance().getText("FixDB"));
			Statement stat = databaseConn.createStatement();
			stat.execute("CREATE TABLE events as SELECT main_seq,SUM(senso) qnt_sensos,SUM(antisenso) qnt_antisensos,min(SUM(senso),SUM(antisenso)) pares FROM events_tmp GROUP BY main_seq");
			stat.execute("DROP TABLE events_tmp");
			stat.execute("vacuum");			
		} catch (SQLException e) {
			Drawer.writeToLog("Database ERROR on loadQuery: "+e.getMessage());
		}
		return;
	}

	public Evento getEvento(){
		try {
			if(rows != null){
				// Itera em cima do set
				if(rows.next()){
					return new Evento(	rows.getString("main_seq"),
							rows.getInt("qnt_sensos"),
							rows.getInt("qnt_antisensos"));
				}
			}
		} catch (SQLException e) {
			Drawer.writeToLog("Database ERROR: "+e.getMessage());
		}
		return null;
	}

	public ResultSet executeQuery(String query){
		int repeats = 0;
		boolean repeat = true;
		while(repeat == true && repeats < maxRepeats){
			try {
				Statement stat = databaseConn.createStatement();
				ResultSet set = stat.executeQuery(query);
				return set;
			} catch (SQLException e) {
				Drawer.writeToLog("Database ERROR: "+e.getMessage());
				fixDatabase();
			}
			repeats++;
		}
		return null;
	}

	public int getSize(){
		Statement stat;
		try {
			stat = databaseConn.createStatement();
			ResultSet rs = stat.executeQuery("SELECT COUNT(*) FROM events");
			return rs.getInt(1);
		} catch (SQLException e) {
			Drawer.writeToLog(e.getMessage());
		}
		return 0;
	}

	public void close(){
		try {
			if(!databaseConn.isClosed()){
				/*if(!rows.isClosed()){
					rows.close();
				}*/
				databaseConn.close();
			}
		} catch (SQLException e) {
			Drawer.writeToLog("Database ERROR: "+e.getMessage());
		}
	}
}
