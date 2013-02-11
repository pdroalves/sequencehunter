//      SocketManager.java
//      
//      Copyright 2013 Pedro Alves <pdroalves@gmail.com>
//      
//		Classe usada para armazenar dados sobre sequencias encontradas pelo Sequence Hunter.
//
//		08/02/2013

package db;

import java.io.Serializable;

public class Event implements Serializable{
	
	private String seq;
	private int qsensos;
	private int qasensos;
	private double qntRel;
	private HunterDatabase dbCincoL;
	
	public Event(){
		
	}
	
	public Event(String s){
		setSeq(s);
	}
	
	public Event(String s,int qs,int qas){
		setSeq(s);
		setQsensos(qs);
		setQasensos(qas);
	}

	public Event(String s,int qs,int qas,double qntRel){
		setSeq(s);
		setQsensos(qs);
		setQasensos(qas);
		setQntRel(qntRel);
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
	public int getQasensos() {
		return qasensos;
	}
	public void setQasensos(int qasensos) {
		this.qasensos = qasensos;
	}
	public double getQntRel() {
		return qntRel;
	}
	public void setQntRel(double qntRel) {
		this.qntRel = qntRel;
	}
	public HunterDatabase getDbCincoL() {
		return dbCincoL;
	}
	public void setDbCincoL(HunterDatabase dbCincoL) {
		this.dbCincoL = dbCincoL;
	}
}
