package com.lnbio.tables.report;

import java.util.ArrayList;
import java.util.Observable;
import java.util.Observer;

import com.lnbio.database.DBManager;
import com.lnbio.hunt.Evento;


@SuppressWarnings("serial")
public class JPartialReportTableModel extends JReportTableModel implements Observer{

	private ArrayList<Evento> data;

	public JPartialReportTableModel(DBManager dbm){
		super(dbm);
		data = centralCutSeqs;
	}

	public JPartialReportTableModel(DBManager dbm,int kind){
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
		return 3;
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
		}
		return obj;
	}

	@Override
	public void update(Observable o, Object arg) {
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
