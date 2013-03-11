package database;

import hunt.Evento;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;

public class DB {
	Connection databaseConn;
	ResultSet rows;
	
	public DB(String databaseFilename){	
	      try {
	    	  // Carrega dinamicamente a lib
	    	  Class.forName("org.sqlite.JDBC");
		      databaseConn = DriverManager.getConnection("jdbc:sqlite:"+databaseFilename);
			} catch (ClassNotFoundException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} catch (SQLException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
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
			// TODO Auto-generated catch block
			e.printStackTrace();
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
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return null;
	}
}
