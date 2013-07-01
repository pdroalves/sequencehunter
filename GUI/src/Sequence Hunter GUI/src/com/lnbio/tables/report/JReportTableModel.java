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
	protected ArrayList<Evento> seqs;

	public JReportTableModel(){

	}

	public JReportTableModel(DBManager dbm){
		super();
		this.dbm = dbm;
		seqs = dbm.getEvents();
		if(dbm.isReady()){
			startLoad();
			super.fireTableDataChanged();
		}
	}
	
	public ArrayList<Evento> getEvents(){
		return dbm.getEvents();
	}

	public int getRowCount() {
		return seqs.size();
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

	protected void startLoad(){
		dbm.startLoad();
	}

	public void load(){
		dbm.load();
	}

	@Override
	public void update(Observable o, Object arg) {
		if(arg != null){
			DBManager dbm = (DBManager) arg;
			if(dbm.isReady()){
				startLoad();
				fireTableDataChanged();
			}
		}		
	}
	
}
