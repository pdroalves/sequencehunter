package tables;

import hunt.Evento;

import java.io.File;
import java.util.ArrayList;
import javax.swing.table.AbstractTableModel;


public class JReportTableModel extends AbstractTableModel{
	
	private String database;
	private ArrayList<Evento> seqs;
	private JReportLoadData jrld;
	private int defaultLoad = 100;
	public JReportTableModel(String db){
		super();
		database = db;
		seqs = new ArrayList<Evento>();
		jrld = new JReportLoadData(database,defaultLoad,seqs);
	}

	@Override
	public int getRowCount() {
		return seqs.size();
	}

	@Override
	public int getColumnCount() {
		return 3;
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
	
	public void loadData(){
		jrld.load();
	}
}
