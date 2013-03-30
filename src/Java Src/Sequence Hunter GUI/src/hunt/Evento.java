package hunt;

public class Evento {
	private String seq;
	private int qsensos;
	private int qantisensos;
	private double qnt_relativa;
	private int pares;
	
	public Evento(){
		
	}
	
	public Evento(String s,int qs,int qas){
		seq = s;
		qsensos = qs;
		qantisensos = qas;
		pares = Math.min(qs, qas);
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
}
