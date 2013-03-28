package database;

import gui.Drawer;
import hunt.Evento;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;

public class DB {
	private Connection databaseConn;
	private ResultSet rows;
	
	public DB(String databaseFilename){	
	      try {
	    	  // Carrega dinamicamente a lib
	    	  Class.forName("org.sqlite.JDBC");
		      databaseConn = DriverManager.getConnection("jdbc:sqlite:"+databaseFilename);
			} catch (ClassNotFoundException e) {
				Drawer.writeToLog("Database ERROR: "+e.getMessage());
			} catch (SQLException e) {
				Drawer.writeToLog("Database ERROR: "+e.getMessage());
			}
	}
	
	public boolean loadQuery(String query){
		Statement stat;
		try {
			stat = databaseConn.createStatement();
			rows = stat.executeQuery(query);
			if(rows != null) 
				return true;
		} catch (SQLException e) {
			Drawer.writeToLog("Database ERROR: "+e.getMessage());
		}
		return false;
	}
	
	public Evento getEvento(){
		try {
			if(rows != null){
				// Itera em cima do set
				if(rows.next()){
					return new Evento(	rows.getString("main_seq"),
										rows.getInt("qnt_sensos"),
										rows.getInt("qnt_antisensos"),
										false);
				}
			}
		} catch (SQLException e) {
			Drawer.writeToLog("Database ERROR: "+e.getMessage());
		}
		return null;
	}
}
