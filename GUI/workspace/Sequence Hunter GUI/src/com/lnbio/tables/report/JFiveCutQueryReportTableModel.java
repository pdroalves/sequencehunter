package com.lnbio.tables.report;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.List;
import java.util.Observer;

import com.lnbio.database.DBManager;
import com.lnbio.hunt.Evento;
import com.lnbio.xml.TranslationsManager;


public class JFiveCutQueryReportTableModel extends JTotalReportTableModel implements Observer{

	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;

	private ArrayList<Evento> seqs;
	private ResultSet data;
	private final String standardQuery = "SELECT SUBSTR(seq,INSTR(seq,\'TARGETFOUND\')-DIST,TAM) fiveCutSeq,COUNT(*) count FROM events_full WHERE central_cut = \'CC\'";
	private int defaultLoad = 100;

	public JFiveCutQueryReportTableModel(DBManager dbm,String centralCutSeq,long dist,long tam) throws Exception {
		// Esse tipo de tabela gera uma query e gerencia diretamente seus elementos
		super(dbm);
		seqs = new ArrayList<Evento>();
		// Get table data
		data = dbm.customQuery(
				standardQuery.replace("CC",centralCutSeq)
				.replace("TARGETFOUND", centralCutSeq)
				.replace("DIST",String.valueOf(dist))
				.replace("TAM",String.valueOf(tam))
				);
		while(	data.next() && 
				seqs.size() < defaultLoad)
			seqs.add(new Evento(data.getString("fiveCutSeq"),data.getInt("count"),0));
	}
	
	public void customHuntLoad(){
		try {
			if(data.next())
				seqs.add(new Evento(data.getString("fiveCutSeq"),data.getInt("count"),0));
		} catch (SQLException e) {
			System.err.println("Erro ao tentar carregar dados para custom hunt: "+e.getMessage());
		}
	}
	
	public void customHuntLoad(long quantity){
		try {
			while(	data.next() && 
					seqs.size() < quantity)
				seqs.add(new Evento(data.getString("fiveCutSeq"),data.getInt("count"),0));
		} catch (SQLException e) {
			System.err.println("Erro ao tentar carregar dados para custom hunt: "+e.getMessage());
		}
	}

	@Override
	public int getRowCount() {
		return seqs.size();
	}

	public String getColumnName(int column) {
		String name = null;
		TranslationsManager tm = TranslationsManager.getInstance();
		switch(column){
		case 0:
			name = tm.getText("reportNumberColumnLabel");
			break;
		case 1:
			name = tm.getText("reportSequenceInfoLabel");
			break;
		case 2:
			name = tm.getText("reportQuantityInfoLabel");
			break;
		}
		return name;
	}
	
	@Override
	public Object getValueAt(int rowIndex, int columnIndex) {
		if(seqs.size() < rowIndex){
			try {
				seqs.add(new Evento(data.getString(1),data.getInt(2),0));
			} catch (SQLException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		Object obj = null;

		switch(columnIndex){
		case 0:
			obj = rowIndex+1;
			break;
		case 1:
			obj = seqs.get(rowIndex).getSeq();
			break;
		case 2:
			obj = seqs.get(rowIndex).getSensos();
			break;
		}
		return obj;
	}


	@Override
	public int getColumnCount() {
		return 3;
	}
	
	public List<Evento> getData(){
		return seqs;
	}
}
