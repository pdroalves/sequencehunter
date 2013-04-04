package tables.report;

import hunt.Evento;

import java.util.ArrayList;
import java.util.Observable;
import java.util.Observer;

import javax.swing.table.AbstractTableModel;

import xml.TranslationsManager;

import database.DBManager;


public abstract class JReportTableModel extends AbstractTableModel implements Observer{

	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;
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
		DBManager dbm = (DBManager) o;
		if(dbm.isReady()){
			startLoad();
			fireTableDataChanged();
		}
	}
}
