package db;
import java.io.Serializable;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.Set;

public class Event implements Serializable{
	
	private String seq = "";
	private int qsensos = 0;
	private int qasensos = 0;
	private double qntRel = 0;
	private Map<String,Event> dbCincoL;
	
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
	public Map<String,Event> getDbCincoL() {
		return dbCincoL;
	}
	public void setDbCincoL(Map<String,Event> dbCincoL) {
		this.dbCincoL = dbCincoL;
	}
	public void setDbCincoL() {
		if(dbCincoL == null){
			dbCincoL = new HashMap<String,Event>();
		}
	}	
	public void printDB(){
		if(dbCincoL != null){
			Set<String> s = dbCincoL.keySet();
			Iterator<String> iterator = s.iterator();
			
			while(iterator.hasNext()){
				String key = iterator.next();
				Event e = dbCincoL.get(key);
				System.out.println("\t"+key+" S:"+e.getQsensos()+" AS:"+e.getQasensos());
				
			}
		}
	}
}