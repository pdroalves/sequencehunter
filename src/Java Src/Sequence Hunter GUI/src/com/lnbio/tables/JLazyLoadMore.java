package com.lnbio.tables;


import java.util.ArrayList;

import com.lnbio.hunt.Library;


public class JLazyLoadMore extends Thread{
	private Library data;
	private int defaultLoad;
	private ArrayList<String> seqs;
	
	public JLazyLoadMore(Library d,int dl,ArrayList<String> db){
		data = d;
		defaultLoad = dl;
		seqs = db;
	}
	
	public void run(){

		for(int i = 0; i < defaultLoad;i++){
			String seq = data.getSeq();
			if(seq != null) seqs.add(seq);
			else return;
		}
	}

}
