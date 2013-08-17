package com.lnbio.tables;


import java.util.ArrayList;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.swing.table.AbstractTableModel;

import com.lnbio.gui.SearchDrawer;
import com.lnbio.hunt.Library;


public class JLibPreviewTableModel extends AbstractTableModel{

	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	private Library data;
	private ArrayList<String> seqs;
	private int defaultLoad = 100;
	private JLibPreviewLoadMore loader;

	public JLibPreviewTableModel(Library lib){
		super();
		data = lib;
		seqs = new ArrayList<String>();
		loader = new JLibPreviewLoadMore(data,defaultLoad,seqs);
		loadMore();
	}

	@Override
	public int getRowCount() {
		return seqs.size();
	}

	@Override
	public int getColumnCount() {
		return 2;
	}

	private String getTargetPattern(){
		String pattern = new String("");
		String originalSeq = SearchDrawer.getTargetSeq();

		for(int i = 0;i < originalSeq.length();i++){
			switch(originalSeq.charAt(i)){
			case 'N':
				pattern = pattern.concat("(.)");
				break;
			default:
				pattern = pattern.concat(String.valueOf(originalSeq.charAt(i)));
				break;
			}
		}

		return pattern;
	}


	private String getAntisenso(String senso){
		String antisenso = new String("");
		for(int i = senso.length()-1; i >= 0;i--){
			switch(senso.charAt(i)){
			case 'A':
				antisenso = antisenso.concat("T");
				break;
			case 'C':
				antisenso = antisenso.concat("G");
				break;
			case 'G':
				antisenso = antisenso.concat("C");
				break;
			case 'T':
				antisenso = antisenso.concat("A");
				break;
			case 'N':
				antisenso = antisenso.concat("N");				
				break;
			case '(':
				antisenso = antisenso.concat(")");				
				break;
			case ')':
				antisenso = antisenso.concat("(");				
				break;				
			default:
				antisenso = antisenso.concat(String.valueOf(senso.charAt(i)));
				break;
			}
		}
		return antisenso;
	}

	@Override
	public Object getValueAt(int rowIndex, int columnIndex) {
		String result = new String("");
		String prefixo;
		String sufixo = "</span>";
		String fiveCutStartSymbol = "<u>";
		String fiveCutEndSymbol = "</u>";
		String fiveCutMiddleEndSymbol;

		switch(columnIndex){
		case 0:
			result = String.valueOf(rowIndex+1);
			break;
		case 1:
			prefixo = "<span style=\"color:blue\">";
			String sequence = seqs.get(rowIndex);
			String targetSequenceFormatted = getTargetPattern();
			//String targetSequenceUnformatted = SearchDrawer.getTargetSeq();

			Pattern patternS = Pattern.compile(targetSequenceFormatted);
			Matcher matcherS = patternS.matcher(sequence);

			Pattern patternAS = Pattern.compile(getAntisenso(targetSequenceFormatted));
			Matcher matcherAS = patternAS.matcher(sequence);

			int tamCL = SearchDrawer.getTamCL();
			int distCL = SearchDrawer.getDistCL();

			StringBuilder builder;

			if(matcherS.find()){
				// Color target sequence
				prefixo = "<span style=\"color:blue\">";
				fiveCutStartSymbol = "<u>";
				fiveCutEndSymbol = "</u>";
				fiveCutMiddleEndSymbol = sufixo+"</u>"+prefixo;
				builder = new StringBuilder();
				builder.append(sequence);
				int start = matcherS.start(0);
				int end = matcherS.end(0);				

				builder.insert(start, prefixo);
				builder.insert(end+prefixo.length(), sufixo);			

				// Color five line cut sequence
				if(	(tamCL != 0 || distCL != 0) && 
						sequence.length() > 0 && 
						start-distCL >= 0 && 
						start-distCL+tamCL <= sequence.length()){
					builder.insert(Math.max(0, start-distCL),fiveCutStartSymbol);
					if(start-distCL+fiveCutStartSymbol.length()+tamCL > start+fiveCutStartSymbol.length()){
						if(start-distCL+fiveCutStartSymbol.length()+prefixo.length()+tamCL > fiveCutStartSymbol.length()+prefixo.length()+end){
							builder.insert(Math.max(0, Math.min(builder.toString().length(), start-distCL+fiveCutStartSymbol.length()+tamCL+prefixo.length()+sufixo.length())),fiveCutEndSymbol);		
						}else{
							builder.insert(Math.max(0, Math.min(builder.toString().length(), start-distCL+fiveCutStartSymbol.length()+tamCL+prefixo.length())),fiveCutMiddleEndSymbol);						
						}
					}else{
						builder.insert(Math.max(0, Math.min(builder.toString().length(), start-distCL+fiveCutStartSymbol.length()+tamCL)),fiveCutEndSymbol);						
					}
				}
				// Final format
				result = "<html><p>";
				result = result.concat(builder.toString());
				result = result.concat("</p></html>");
			}else{
				if(matcherAS.find()){
					prefixo = "<span style=\"color:red\">";
					fiveCutStartSymbol = "</u>";
					fiveCutEndSymbol = "<u>";
					fiveCutMiddleEndSymbol = sufixo+"<u>"+prefixo;
					builder = new StringBuilder();
					builder.append(sequence);
					int start = matcherAS.start(0);
					int end = matcherAS.end(0);
					builder.insert(start, prefixo);
					builder.insert(end+prefixo.length(), sufixo);

					// Color five line cut sequence
					if(	(tamCL != 0 || distCL != 0) && 
							sequence.length() > 0 && 
							end+distCL < sequence.length() && 
							end+distCL-tamCL >= 0){
						builder.insert(Math.max(0, prefixo.length()+end+sufixo.length()+distCL),fiveCutStartSymbol);
						if(prefixo.length()+end+sufixo.length()+distCL - tamCL <  prefixo.length()+end+sufixo.length()){
							if(prefixo.length()+end+sufixo.length()+distCL-tamCL-sufixo.length() <  start+prefixo.length()){
								builder.insert(Math.max(0, Math.min(builder.toString().length(), prefixo.length()+end+sufixo.length()+distCL-sufixo.length()-tamCL-prefixo.length())),fiveCutEndSymbol);		
							}else{
								builder.insert(Math.max(0, Math.min(builder.toString().length(),  prefixo.length()+end+sufixo.length()+distCL-sufixo.length()-tamCL)),fiveCutMiddleEndSymbol);						
							}
						}else{
							builder.insert(Math.max(0, Math.min(builder.toString().length(), prefixo.length()+end+sufixo.length()+distCL-tamCL)),fiveCutEndSymbol);						
						}
					}
					// Final format
					result = result.concat("<html><p>");
					result = result.concat(builder.toString());
					System.out.println(builder.toString());
					result = result.concat("</p></html>");
				}else
					result = sequence;
			}
			break;
		}
		return result;
	}

	public void loadMore(){
		loader.load();
		return;
	}

	@Override
	public String getColumnName(int column) {
		String result = null;
		switch(column){
		case 0:
			result = "#";
			break;
		case 1:
			result = "Library Preview";
			break;
		}
		return result;
	}

}
