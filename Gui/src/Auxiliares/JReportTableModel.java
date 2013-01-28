package Auxiliares;

import java.io.File;
import java.util.ArrayList;
import javax.swing.table.AbstractTableModel;

public class JReportTableModel extends AbstractTableModel{
	
	private File data;
	private ArrayList<String> seqs;
	private ArrayList<Integer> freqs;
	
	public JReportTableModel(File f){
		super();
		data = f;
		seqs = new ArrayList<String>();
		freqs = new ArrayList<Integer>();
		loadData();
	}

	@Override
	public int getRowCount() {
		return seqs.size();
	}

	@Override
	public int getColumnCount() {
		return 2;
	}

	@Override
	public Object getValueAt(int rowIndex, int columnIndex) {
		Object obj = null;
		switch(columnIndex){
		case 0:
			obj = seqs.get(rowIndex);
			break;
		case 1:
			obj = freqs.get(rowIndex);
			break;
		}
		return obj;
	}
	
	@Override
	public String getColumnName(int column) {
		String name = null;
		switch(column){
		case 0:
			name = "Sequences";
			break;
		case 1:
			name = "#Paired";
			break;
		}
		return name;
	}

	public void loadData(){
		JReportLoadData jrld = new JReportLoadData(data,seqs,freqs);
		jrld.load();
		return;
	}

}