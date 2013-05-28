package database;

import gui.Drawer;
import hunt.Evento;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;

public class DB {
	private static Connection databaseConn;
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
		try {
			Statement stat = databaseConn.createStatement();
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
							rows.getInt("qnt_antisensos"));
				}
			}
		} catch (SQLException e) {
			Drawer.writeToLog("Database ERROR: "+e.getMessage());
		}
		return null;
	}

	public ResultSet executeQuery(String query){
		try {
			Statement stat = databaseConn.createStatement();
			ResultSet set = stat.executeQuery(query);
			return set;
		} catch (SQLException e) {
			Drawer.writeToLog("Database ERROR: "+e.getMessage());
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
