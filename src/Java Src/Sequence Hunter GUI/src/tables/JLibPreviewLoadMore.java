package tables;

import hunt.Library;

import java.util.ArrayList;


public class JLibPreviewLoadMore{
	private Library data;
	private int defaultLoad;
	private ArrayList<String> seqs;
	
	public JLibPreviewLoadMore(Library d,int dl,ArrayList<String> db){
		data = d;
		defaultLoad = dl;
		seqs = db;
	}
	
	public void load(){
		for(int i = 0; i < defaultLoad;i++){
			String seq = data.getSeq();
			if(seq != null) 
				seqs.add(seq);
			else 
				return;
		}
	}

}
