package Auxiliares;

import java.util.ArrayList;
import javax.swing.table.AbstractTableModel;

public class JLazyTableModel extends AbstractTableModel{
	
	private Library data;
	private ArrayList<String> seqs;
	private int defaultLoad = 100;
	
	public JLazyTableModel(Library lib){
		super();
		data = lib;
		seqs = new ArrayList<String>();
		loadMore();
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
	
	public void loadMore(){
		for(int i = 0; i < defaultLoad;i++)
			seqs.add(data.getSeq());
		return;
	}

}
