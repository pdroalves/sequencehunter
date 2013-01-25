package Auxiliares;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.Scanner;

public class JReportLoadData{
	private File data;
 	private ArrayList<String> seqs;
	private ArrayList<Integer> freqs;
	
	public JReportLoadData(File f,ArrayList<String> seqs,ArrayList<Integer> freqs){
		this.data = f;
		this.seqs = seqs;
		this.freqs = freqs;
	}
	
	public void load(){
		String seqRead;
		Integer freqRead;
		
		try{
			Scanner sc = new Scanner(data);
			
			if(data.canRead()){
				while(sc.hasNext()){
					seqRead = sc.next();
					if(checkValid(seqRead) && sc.hasNextInt()){
						freqRead = sc.nextInt();
						seqs.add(seqRead);
						freqs.add(freqRead);
					}
				}
			}
		}catch(FileNotFoundException e){
			e.printStackTrace();
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

}
