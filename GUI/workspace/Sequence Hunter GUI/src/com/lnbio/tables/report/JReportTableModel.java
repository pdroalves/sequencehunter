package com.lnbio.tables.report;


import java.util.ArrayList;
import java.util.Observable;
import java.util.Observer;

import javax.swing.table.AbstractTableModel;

import com.lnbio.database.DBManager;
import com.lnbio.hunt.Evento;
import com.lnbio.xml.TranslationsManager;




@SuppressWarnings("serial")
public abstract class JReportTableModel extends AbstractTableModel implements Observer{
	private DBManager dbm;
	protected ArrayList<Evento> centralCutSeqs;
	protected ArrayList<Evento> fiveCutSeqs;

	public JReportTableModel(){

	}

	public JReportTableModel(DBManager dbm){
		super();
		this.dbm = dbm;
		centralCutSeqs = dbm.getCentralCutEvents();
		fiveCutSeqs = dbm.getCentralCutEvents();
		if(dbm.isReady()){
			startCentralCutLoad();
			super.fireTableDataChanged();
		}
	}
	
	public ArrayList<Evento> getCentralCutEvents(){
		return dbm.getCentralCutEvents();
	}

	public int getRowCount() {
		return centralCutSeqs.size();
	}

	public abstract int getColumnCount();

	@Override
	public Object getValueAt(int rowIndex, int columnIndex) {
		Object obj = null;

		return obj;
	}

	public String getColumnName(int column) {
		String name = null;
		TranslationsManager tm = TranslationsManager.getInstance();
		switch(column){
		case 0:
			name = tm.getText("reportNumberColumnLabel");
			break;
		case 1:
			name = tm.getText("reportSequencesColumnLabel");
			break;
		case 2:
			name = tm.getText("reportPairedColumnLabel");
			break;
		}
		return name;
	}

	protected void startCentralCutLoad(){
		dbm.startCentralCutLoad();
	}
	
	protected void startFiveCutLoad(){
		dbm.startFiveCutLoad();
	}

	public void centralCutLoad(){
		dbm.centralCutLoad();
	}
	
	public void fiveCutLoad(){
		dbm.fiveCutLoad();
	}

	@Override
	public void update(Observable o, Object arg) {
		if(arg != null){
			DBManager dbm = (DBManager) arg;
			if(dbm.isReady()){
				startCentralCutLoad();
				startFiveCutLoad();
				fireTableDataChanged();
			}
		}		
	}
	
}
