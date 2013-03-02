package tables;

import gui.Drawer;
import hunt.Evento;

import java.io.BufferedInputStream;
import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.ArrayList;
import java.util.InputMismatchException;
import java.util.Scanner;


public class JReportLoadData{
	private File data;
 	private ArrayList<Evento> seqs;
	
	public JReportLoadData(File f,ArrayList<Evento> e){
		this.data = f;
		seqs = e;
	}
	
	public void load(){
		int totalSeqs;
		boolean regiao5l;
		String seqRead;
		int seqSize;
		int qsensos;
		int qasensos;
		Evento main;
		Evento sub;
		int statusSUBHT;
		
		try{
			Scanner sc = new Scanner(new FileInputStream(data));
			if(data.canRead()){
				totalSeqs = sc.nextInt();
				if(sc.nextInt() == 1){
					regiao5l = true;
				}else{
					regiao5l = false;
				}
				while(sc.hasNext()){
					seqSize = sc.nextInt();
					seqRead = sc.next();
					qsensos = sc.nextInt();
					qasensos = sc.nextInt();
					main = new Evento(seqRead,qsensos,qasensos,regiao5l);
					
					if(regiao5l){
						statusSUBHT = sc.nextInt();
						while(statusSUBHT == 1){
							seqSize = sc.nextInt();
							seqRead = sc.next();
							qsensos = sc.nextInt();
							qasensos = sc.nextInt();

							if(checkValid(seqRead)){
								sub = new Evento(seqRead,qsensos,qasensos,false);
								main.addTo5l(sub);
							}
							statusSUBHT = sc.nextInt();
						}
					}

					if(checkValid(seqRead)){
						seqs.add(main);
					}
				}
			}
		}catch(FileNotFoundException error){
			error.printStackTrace();
			Drawer.writeToLog("File " + data.getAbsolutePath() + " can not be read.");
		}catch (InputMismatchException error) {
			error.printStackTrace();
			Drawer.writeToLog("File " + data.getAbsolutePath() + " can not be read.");
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
