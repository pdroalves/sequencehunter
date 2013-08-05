package com.lnbio.tables.report;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import com.lnbio.database.DBManager;
import com.lnbio.hunt.Evento;
import com.lnbio.xml.TranslationsManager;

@SuppressWarnings("serial")
public class JFullSeqReportTableModel extends JReportTableModel {
	private int dist;
	private int tam;
	private String centralCutSeq;
	private ArrayList<Evento> seqs;
	private ResultSet data;
	private int defaultLoad = 100;
	private final String standardQuery = "SELECT seq fiveCutSeq,COUNT(*) count FROM events_full WHERE central_cut = \'CC\' LIMIT 100";

	public JFullSeqReportTableModel(DBManager dbm,String centralCutSeq,int dist,int tam) throws Exception {
		// Esse tipo de tabela gera uma query e gerencia diretamente seus elementos
		super(dbm);
		this.centralCutSeq = centralCutSeq;
		this.dist = dist;
		this.tam = tam;
		seqs = new ArrayList<Evento>();
		startLoad();
	}


	private void startLoad() throws SQLException{
		seqs.clear();
		// Get table data
		data = super.dbm.customQuery(
				standardQuery.replace("CC",centralCutSeq)
				.replace("TARGETFOUND", centralCutSeq)
				.replace("DIST",String.valueOf(dist))
				.replace("TAM",String.valueOf(tam))
				);
		while(data.next() && seqs.size() < defaultLoad)
			seqs.add(new Evento(data.getString("fiveCutSeq"),0,0));
	}

	public void update(String newCentralCutSeq,int newDist,int newTam) {
		centralCutSeq = newCentralCutSeq;
		this.dist = newDist;
		this.tam = newTam;
		super.fireTableDataChanged();
	}

	public boolean isReady(){
		try{
			String sequence = seqs.get(0).getSeq();

			Pattern patternS = Pattern.compile(centralCutSeq);
			Matcher matcherS = patternS.matcher(sequence);

			StringBuilder builder;

			if(matcherS.find()){
				builder = new StringBuilder();
				builder.append(sequence);
				int start = matcherS.start(0);
				if(start-dist >= 0 &&
						start-dist+tam < sequence.length() &&
						dist != 0 &&
						tam != 0){
					return true;
				}
			}
		}catch(Exception e){

		}
		return false;
	}

	@Override
	public int getRowCount() {
		return seqs.size();
	}

	@Override
	public Object getValueAt(int rowIndex, int columnIndex) {
		try {
			while(seqs.size() < rowIndex && data.next()){
				seqs.add(new Evento(data.getString(1),data.getInt(2),0));
			}
		} catch (SQLException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

		String result = new String("");
		switch(columnIndex){
		case 0:
			result = String.valueOf(rowIndex+1);
			break;
		case 1:
			String sequence = seqs.get(rowIndex).getSeq();

			Pattern patternS = Pattern.compile(centralCutSeq);
			Matcher matcherS = patternS.matcher(sequence);

			StringBuilder builder;

			if(matcherS.find()){
				String prefixo = "<span style=\"font-weight: bold\">";
				String sufixo = "</span>";
				builder = new StringBuilder();
				builder.append(sequence);
				int start = matcherS.start(0);
				int end = matcherS.end(0);
				if(start-dist >= 0 &&
						start-dist+tam < sequence.length()){
					builder.insert(start-dist, prefixo);
					builder.insert(start-dist+tam+prefixo.length(), sufixo);
					result = result.concat("<html><p>");
					result = result.concat(builder.toString());
					result = result.concat("</p></html>");
				}else{
					dist = tam = 0;
					super.fireTableDataChanged();
				}
			}else{
				result = sequence;
			}
			break;
		}
		return result;
	}

	@Override
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
	public int getColumnCount() {
		return 2;
	}

	public void customHuntLoad(){
		try {
			if(data.next())
				seqs.add(new Evento(data.getString("fiveCutSeq"),0,0));
		} catch (SQLException e) {
			System.err.println("Erro ao tentar carregar dados para custom hunt: "+e.getMessage());
		}
	}

}
