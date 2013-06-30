package com.lnbio.tables.report;

import java.util.Observable;
import java.util.Observer;

import com.lnbio.database.DBManager;
import com.lnbio.xml.TranslationsManager;



public class JTotalReportTableModel extends JReportTableModel implements Observer{
	
	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;

	public JTotalReportTableModel(){
		super();
	}
	
	public JTotalReportTableModel(DBManager dbm){
		super(dbm);
	}
	
	public int getColumnCount() {
		return 6;
	}
	
	public String getColumnName(int column) {
		String name = null;
		TranslationsManager tm = TranslationsManager.getInstance();
		switch(column){
		case 3:
			name = tm.getText("reportSensosColumnLabel");
			break;
		case 4:
			name = tm.getText("reportAntisensosColumnLabel");
			break;
		case 5:
			name = tm.getText("reportRelativeColumnLabel");
			break;
		default:
			name = super.getColumnName(column);
			break;
		}
		return name;
	}
	
	@Override
	public Object getValueAt(int rowIndex, int columnIndex) {
		Object obj = null;
		
		while(seqs.size() < rowIndex){
			load();		
		}
		
		switch(columnIndex){
			case 0:
				obj = rowIndex+1;
			break;
			case 1:
				obj = seqs.get(rowIndex).getSeq();
			break;
			case 2:
				obj = seqs.get(rowIndex).getPares();
			break;
			case 3:
				obj = seqs.get(rowIndex).getSensos();
			break;
			case 4:
				obj = seqs.get(rowIndex).getAntisensos();
			break;
			case 5:
				obj =  String.format("%.4f",seqs.get(rowIndex).getRelativeFreq());
			break;
		}
		return obj;
	}

	@Override
	public void update(Observable observable, Object arg) {
		// TODO Auto-generated method stub
		
	}

}
