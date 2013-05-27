package tables.report;

import histogram.EventHistogram;

import java.io.File;

import database.DBManager;

public class ReportFactory {

	public TabledReport createTabledReport(String name,DBManager dbm,JReportTableModel jtrtm){
		return new TabledReport(name,dbm,jtrtm);		
	}
	
	public TextReport createTextReport(File txtFile){
		return new TextReport(txtFile);
	}
	
}
