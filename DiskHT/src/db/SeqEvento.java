package db;

import java.io.File;
import java.io.IOException;
import java.util.HashMap;

public class SeqEvento {
	private HunterDatabase db;
	private String seq;
	private int qsensos;
	private int qantisensos;
	private double qntRelativa;
	
	public SeqEvento(){
	}
	
	public SeqEvento(String s){
		setSeq(s);
	}
	
	public SeqEvento(String s,int qs,int qa){
		setQsensos(qs);
		setQantisensos(qa);
	}
	
	public SeqEvento(String s,int qs,int qa,double qntR){
		setQsensos(qs);
		setQantisensos(qa);
		setQntRelativa(qntR);
	}

	public String getSeq() {
		return seq;
	}

	public void setSeq(String seq) {
		this.seq = seq;
	}

	public int getQsensos() {
		return qsensos;
	}

	public void setQsensos(int qsensos) {
		this.qsensos = qsensos;
	}

	public int getQantisensos() {
		return qantisensos;
	}

	public void setQantisensos(int qantisensos) {
		this.qantisensos = qantisensos;
	}

	public double getQntRelativa() {
		return qntRelativa;
	}

	public void setQntRelativa(double qntRelativa) {
		this.qntRelativa = qntRelativa;
	}
	
	public void setCincoL(boolean b){
		if(b){
			db = new HunterDatabase("cincoL",new File(seq));
		}
	}
	
	public void addCincoL(String s){
		
	}
	
}
