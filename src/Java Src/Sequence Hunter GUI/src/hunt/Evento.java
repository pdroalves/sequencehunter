package hunt;

import myTypeData.GenType;

public class Evento extends GenType{
	private String seq;
	private int qsensos;
	private int qantisensos;
	private int pares;
	private int mode = 0;
	static public final int VALUE_PARES_ABS = 0;
	static public final int VALUE_PARES_REL = 1;
	static public final int VALUE_SENSOS_ABS = 2;
	static public final int VALUE_SENSOS_REL = 3;
	static public final int VALUE_ANTISENSO_ABS = 4;
	static public final int VALUE_ANTISENSO_REL = 5;

	public Evento(String s,int qs,int qas){
		super(s,Math.min(qs, qas));
		seq = s;
		qsensos = qs;
		qantisensos = qas;
		pares = Math.min(qs, qas);
	}

	public void setMode(int mode){
		this.mode = mode;
	}

	@Override
	public int getValue() {
		switch(mode){
		case VALUE_PARES_ABS:
			return this.getPares();
		case VALUE_PARES_REL:			
			return Math.round(this.getRelativeFreq());
		default:
			return this.getPares();
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

}
