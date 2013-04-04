package tables.report;

import java.util.Observer;

import xml.TranslationsManager;

import database.DBManager;

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
		return 5;
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
		}
		return obj;
	}

}
