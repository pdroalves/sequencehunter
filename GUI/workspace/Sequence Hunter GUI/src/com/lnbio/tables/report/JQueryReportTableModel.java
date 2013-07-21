package com.lnbio.tables.report;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.Observer;

import com.lnbio.gui.Drawer;
import com.lnbio.hunt.Evento;


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
	protected void startCentralCutLoad() {
		// TODO Auto-generated method stub
		super.startCentralCutLoad();
	}

	@Override
	public void centralCutLoad() {
		// TODO Auto-generated method stub
		super.centralCutLoad();
	}

	@Override
	public int getColumnCount() {
		return 5;
	}
}
