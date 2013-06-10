package com.lnbio.tables.report;

import java.util.Observable;
import java.util.Observer;

import com.lnbio.database.DBManager;


@SuppressWarnings("serial")
public class JPartialReportTableModel extends JReportTableModel implements Observer{

	public JPartialReportTableModel(DBManager dbm){
		super(dbm);
	}

	public int getColumnCount() {
		return 3;
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
		}
		return obj;
	}

	@Override
	public void update(Observable o, Object arg) {
		if(arg != null){
			DBManager dbm = (DBManager) arg;
			if(dbm.isReady()){
				super.startLoad();
				super.fireTableDataChanged();
			}
		}
	}
}
