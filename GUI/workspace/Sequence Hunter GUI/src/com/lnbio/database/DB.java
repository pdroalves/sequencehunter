package com.lnbio.database;


import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;

import com.lnbio.gui.Drawer;
import com.lnbio.hunt.Evento;
import com.lnbio.xml.TranslationsManager;


public class DB {
	private Connection databaseConn;
	private ResultSet centralCutRows;
	private ResultSet fiveCutRows;
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

	public boolean loadCentralCutQuery(String centralCutQuery){
		boolean repeat = true;
		int repeats = 0;
		Statement stat = null;
		while(repeat == true && repeats < maxRepeats){
			try {
				stat = databaseConn.createStatement();
				centralCutRows = stat.executeQuery(centralCutQuery);
				if(centralCutRows != null ) 
					return true;
			} catch (SQLException e) {
				Drawer.writeToLog("Database ERROR on loadQuery: "+e.getMessage());
				if(e.getMessage().contains("full")){
					Drawer.writeToLog(TranslationsManager.getInstance().getText("DiskFullForTmpFile"));
					Drawer.writeToLog(TranslationsManager.getInstance().getText("LowDiskSpaceMode"));
					centralCutQuery.concat(" limit 1000");
				}else if(e.getMessage().contains("no such table")){
					fixDatabase();
				}
			}
			repeats++;
		}
		return false;
	}
	
	public boolean loadFiveCutQuery(String fiveCutQuery){
		boolean repeat = true;
		int repeats = 0;
		Statement stat = null;
		while(repeat == true && repeats < maxRepeats){
			try {
				stat = databaseConn.createStatement();
				fiveCutRows = stat.executeQuery(fiveCutQuery);
				if(fiveCutRows != null ) 
					return true;
			} catch (SQLException e) {
				Drawer.writeToLog("Database ERROR on loadQuery: "+e.getMessage());
				if(e.getMessage().contains("full")){
					Drawer.writeToLog(TranslationsManager.getInstance().getText("DiskFullForTmpFile"));
					Drawer.writeToLog(TranslationsManager.getInstance().getText("LowDiskSpaceMode"));
					fiveCutQuery.concat(" limit 1000");
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
			stat.execute("CREATE TABLE events as SELECT main_seq,SUM(senso) qnt_sensos,SUM(antisenso) qnt_antisensos FROM events_tmp GROUP BY main_seq");
			stat.execute("DROP TABLE events_tmp");
			stat.execute("CREATE TABLE events_5l as SELECT seq,SUM(senso) qnt_sensos,SUM(antisenso) qnt_antisensos FROM events_5l_tmp GROUP BY seq");
			stat.execute("DROP TABLE events_5l_tmp");
			stat.execute("vacuum");			
			Drawer.writeToLog(TranslationsManager.getInstance().getText("FixDBDone"));
		} catch (SQLException e) {
			Drawer.writeToLog("Database ERROR on loadQuery: "+e.getMessage());
		}
		return;
	}

	public Evento getCentralCutEvento(){
		try {
			if(centralCutRows != null){
				// Itera em cima do set
				if(centralCutRows.next()){
					return new Evento(	centralCutRows.getString("main_seq"),
							centralCutRows.getInt("qnt_sensos"),
							centralCutRows.getInt("qnt_antisensos"));
				}
			}
		} catch (SQLException e) {
			Drawer.writeToLog("Database ERROR: "+e.getMessage());
		}
		return null;
	}
	
	public Evento getFiveCutEvento(){
		try {
			if(fiveCutRows != null){
				// Itera em cima do set
				if(fiveCutRows.next()){
					return new Evento(	fiveCutRows.getString("seq"),
							fiveCutRows.getInt("qnt_sensos"),
							fiveCutRows.getInt("qnt_antisensos"));
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

	public int getCentralCutSize(){
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
	
	public int getFiveCutSize(){
		Statement stat;
		try {
			stat = databaseConn.createStatement();
			ResultSet rs = stat.executeQuery("SELECT COUNT(*) FROM events_5l");
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
