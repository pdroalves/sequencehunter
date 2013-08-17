package com.lnbio.hunt;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;

public class Library {
	
	private File data;
	private long fileSize;
	private BufferedReader br;
	private ArrayList<String> db;
	
	public Library(File f) throws FileNotFoundException{
		data = f;
		fileSize = f.length();
		br = new BufferedReader(new FileReader(f));
		startDB();
	}
	
	public boolean canRead(){
		return data.canRead();
	}
	
	public String getPath(){
		return data.getAbsolutePath();
	}
	
	public String getFilename(){
		return data.getName();
	}
	
	public long getFileSize(){
		return fileSize;
	}
	
	private void startDB(){
		db = new ArrayList<String>();
		db.add("A");
		db.add("C");
		db.add("G");
		db.add("T");
		db.add("N");
	}
		
	public String getSeq(){
		String seqRead = new String("");
		Boolean founded = false;
		try{
			while(!founded && seqRead != null){
				seqRead = br.readLine();
				if(checkValid(seqRead)){
					founded = true;
				}
			}
			return seqRead;
		}catch(IOException e){
			return "";
		}
	}
		private boolean checkValid(String seq){
		if(seq != null){
		for(int i=0;i<seq.length();i++){
			if(!(seq.charAt(i) == 'A' ||
					seq.charAt(i) == 'C' ||
					seq.charAt(i) == 'G' ||
					seq.charAt(i) == 'T' ||
					seq.charAt(i) == 'N')){
				return false;
			}
		}
		}else{
			return false;
		}
		return true;
	}
	
	public void close(){
		try{
			br.close();
		}catch(IOException e){
			
		}
	}

}
