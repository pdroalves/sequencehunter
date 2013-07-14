package com.lnbio.tables.report;


import java.io.File;

import com.lnbio.database.DBManager;
import com.lnbio.histogram.EventHistogram;


public class ReportFactory {

	public TabledReport createTabledReport(String name,DBManager dbm,JReportTableModel jtrtm){
		return new TabledReport(name,dbm,jtrtm);		
	}
	
	public TextReport createTextReport(File txtFile){
		return new TextReport(txtFile);
	}
	
}
