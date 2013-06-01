package tables.report;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.Observer;

import gui.Drawer;
import hunt.Evento;

public class JQueryReportTableModel extends JTotalReportTableModel implements Observer{

	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;

	private ArrayList<Evento> seqs;
	private ResultSet mainSet;
	
	public JQueryReportTableModel(ResultSet rs) {
		// Esse tipo de tabela carrega os dados direto de um ResultSet
		super();
		mainSet = rs;
		seqs = new ArrayList<Evento>();
	}
	
	@Override
	public int getRowCount() {
		return seqs.size();
	}

	@Override
	public Object getValueAt(int rowIndex, int columnIndex) {
		// TODO Auto-generated method stub
		return super.getValueAt(rowIndex, columnIndex);
	}

	@Override
	protected void startLoad() {
		// TODO Auto-generated method stub
		super.startLoad();
	}

	@Override
	public void load() {
		// TODO Auto-generated method stub
		super.load();
	}

	@Override
	public int getColumnCount() {
		return 5;
	}
}
