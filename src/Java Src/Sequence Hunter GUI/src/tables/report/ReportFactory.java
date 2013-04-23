package tables.report;

import java.io.File;

import database.DBManager;

public class ReportFactory {

	public TabledReport createTabledReport(DBManager dbm,JReportTableModel jtrtm){
		return new TabledReport(dbm,jtrtm);
		
	}
	
	public TextReport createTextReport(File txtFile){
		return new TextReport(txtFile);
	}
	
}
