package Hunt;

import java.util.ArrayList;

public class Evento {
	private String seq;
	private int qsensos;
	private int qantisensos;
	private double qnt_relativa;
	private int pares;
	private ArrayList<Evento> regiao5l;
	
	public Evento(){
		
	}
	
	public Evento(String s,int qs,int qas,boolean r5l){
		seq = s;
		qsensos = qs;
		qantisensos = qas;
		pares = Math.min(qs, qas);
		if(r5l){
			regiao5l = new ArrayList<Evento>();
		}
	}
	
	public void setSeq(String s){
		seq = s;
	}
	
	public String getSeq(){
		return seq;
	}
	
	public void setPares(int n){
		pares = n;
	}
	
	public int getPares(){
		return pares;
	}
	
	public void setSensos(int n){
		qsensos = n;
	}
	
	public int getSensos(){
		return qsensos;
	}
	
	public void setAntisensos(int n){
		qantisensos = n;
	}
	
	public int getAntisensos(){
		return qantisensos;
	}
	
	public void setQntRelativa(double q){
		qnt_relativa = q;
	}
	
	public double getQntRelativa(){
		return qnt_relativa;
	}
	
	public void have5l(boolean b){
		if(b){
			if(regiao5l == null){
				regiao5l = new ArrayList<Evento>();				
			}
		}
	}
	
	public void addTo5l(Evento e){
		regiao5l.add(e);
		return;
	}

}
