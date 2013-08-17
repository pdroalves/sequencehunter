package com.lnbio.tables.report;

import java.util.ArrayList;
import java.util.Observable;
import java.util.Observer;

import com.lnbio.database.DBManager;
import com.lnbio.hunt.Evento;
import com.lnbio.xml.TranslationsManager;



public class JTotalReportTableModel extends JReportTableModel implements Observer{
	
	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	private ArrayList<Evento> data;

	public JTotalReportTableModel(){
		super();
		data = centralCutSeqs;
	}
	
	public JTotalReportTableModel(DBManager dbm){
		super(dbm);
		data = centralCutSeqs;
	}
	
	public JTotalReportTableModel(DBManager dbm,int kind){
		super(dbm);
		super.setKind(kind);
		switch(kind){
		case CENTRAL_CUT:
			data = centralCutSeqs;
			break;
		case FIVE_CUT:
			data = fiveCutSeqs;
			break;
		}
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
		
		while(data.size() < rowIndex){
			switch(super.getKind()){
			case CENTRAL_CUT:
				centralCutLoad();	
				break;
			case FIVE_CUT:
				fiveCutLoad();
				break;
			}	
		}
		
		switch(columnIndex){
			case 0:
				obj = rowIndex+1;
			break;
			case 1:
				obj = data.get(rowIndex).getSeq();
			break;
			case 2:
				obj = data.get(rowIndex).getPares();
			break;
			case 3:
				obj = data.get(rowIndex).getSensos();
			break;
			case 4:
				obj = data.get(rowIndex).getAntisensos();
			break;
			case 5:
				obj =  String.format("%.4f",data.get(rowIndex).getRelativeFreq());
			break;
		}
		return obj;
	}

	@Override
	public void update(Observable observable, Object arg) {
		if(arg != null){
			DBManager dbm = (DBManager) arg;
			if(dbm.isReady()){
				switch(super.getKind()){
				case CENTRAL_CUT:
					super.startCentralCutLoad();
					break;
				case FIVE_CUT:
					super.startFiveCutLoad();
					break;
				}	
				super.fireTableDataChanged();
			}
		}
	}

}
