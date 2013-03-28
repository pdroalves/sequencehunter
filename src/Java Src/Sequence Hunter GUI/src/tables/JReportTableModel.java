package tables;

import hunt.Evento;

import java.util.ArrayList;

import javax.swing.table.AbstractTableModel;

import database.DBManager;


public abstract class JReportTableModel extends AbstractTableModel{
	
	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	protected ArrayList<Evento> seqs;
	protected int defaultLoad = 100;
	
	public JReportTableModel(DBManager dbm){
		super();
		seqs = new ArrayList<Evento>();
		if(dbm.isReady()){
			startLoad();
			super.fireTableDataChanged();
		}
	}

	public int getRowCount() {
		return seqs.size();
	}

	public int getColumnCount() {
		return 3;
	}

	@Override
	public Object getValueAt(int rowIndex, int columnIndex) {
		Object obj = null;
	
		return obj;
	}
	
	public String getColumnName(int column) {
		String name = null;
		switch(column){
		case 0:
			name = "#";
			break;
		case 1:
			name = "Sequences";
			break;
		case 2:
			name = "#Paired";
			break;
		}
		return name;
	}
	
	public abstract void loadData();
	
	protected void startLoad(){
		for(int i = 0; i < defaultLoad;i++)
			loadData();
	}
}
