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
		return 1;
	}

	@Override
	public Object getValueAt(int rowIndex, int columnIndex) {
		return seqs.get(rowIndex);
	}
	
	public void loadData(){
		(new JReportLoadData(data,seqs,freqs)).start();
		return;
	}

}
