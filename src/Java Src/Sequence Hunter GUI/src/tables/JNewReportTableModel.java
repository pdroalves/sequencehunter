package tables;

import java.util.Observable;
import java.util.Observer;

import database.DBManager;

public class JNewReportTableModel extends JReportTableModel implements Observer{
	
	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;

	private JReportLoadData jrld;
	
	public JNewReportTableModel(DBManager dbm){
		super(dbm);
		jrld = new JReportLoadData(defaultLoad,seqs,dbm);
	}
	
	@Override
	public Object getValueAt(int rowIndex, int columnIndex) {
		Object obj = null;
		
		while(seqs.size() < rowIndex){
			jrld.load();		
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
	public void loadData(){
		jrld.load();
	}
	
	@Override
	public void update(Observable o, Object arg) {
		DBManager dbm = (DBManager) o;
		if(dbm.isReady()){
			super.startLoad();
			super.fireTableDataChanged();
		}
	}
}
