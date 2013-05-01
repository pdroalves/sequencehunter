package tables;

import gui.SearchDrawer;
import hunt.Library;

import java.util.ArrayList;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.swing.table.AbstractTableModel;


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
		switch(columnIndex){
		case 0:
			result = String.valueOf(rowIndex+1);
			break;
		case 1:
			String sequence = seqs.get(rowIndex);
			
			Pattern patternS = Pattern.compile(getTargetPattern());
			Matcher matcherS = patternS.matcher(sequence);
			
			Pattern patternAS = Pattern.compile(getAntisenso(getTargetPattern()));
			Matcher matcherAS = patternAS.matcher(sequence);
			
			StringBuilder builder;
			
			if(matcherS.find()){
				String prefixo = "<span style=\"color:blue\">";
				String sufixo = "</span>";
				builder = new StringBuilder();
				builder.append(sequence);
				for(int g = 0; g < 1;g++){
					int start = matcherS.start(g);
					int end = matcherS.end(g);
					builder.insert(start, prefixo);
					builder.insert(end+prefixo.length(), sufixo);
				}
				result = result.concat("<html><p>");
				result = result.concat(builder.toString());
				result = result.concat("</p></html>");
			}else{
				if(matcherAS.find()){
					String prefixo = "<span style=\"color:red\">";
					String sufixo = "</span>";
					builder = new StringBuilder();
					builder.append(sequence);
					for(int g = 0; g < 1;g++){
						int start = matcherAS.start(g);
						int end = matcherAS.end(g);
						builder.insert(start, prefixo);
						builder.insert(end+prefixo.length(), sufixo);
					}
					result = result.concat("<html><p>");
					result = result.concat(builder.toString());
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
