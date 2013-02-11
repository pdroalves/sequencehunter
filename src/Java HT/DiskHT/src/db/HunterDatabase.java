//      SocketManager.java
//      
//      Copyright 2013 Pedro Alves <pdroalves@gmail.com>
//      
//		Classe que implementa MapDB para armazenamento e leitura de dados do banco de dados com objetos do tipo Event.
//		Serve de auxilio para o Sequence Hunter.
//
//		08/02/2013

package db;

import java.io.File;
import java.util.Iterator;
import java.util.Map;
import java.util.Set;
import org.mapdb.*;


public class HunterDatabase {
	private DB db;
	private Map<String, Event> dbMap;
	private boolean regiaoCincoLSupport = false;

	public HunterDatabase(String dbName,File f){
		db = DBMaker.newFileDB(f)
				.asyncWriteDisable()
				.make();
		dbMap = db.getHashMap(dbName);
	}
	
	public void add(String key,Event value){
		Event returnSE = dbMap.get(key);
		if(returnSE != null){
			dbMap.put(key, updateValue(value,returnSE));
		}else{
			dbMap.put(key, value);
		}
	}
	
	public void commit(){
		System.out.println("Commit...");
		db.commit();
		System.out.println("Commit done.");
	}
	
	private Event updateValue(Event novo, Event velho){
		Event atualizado = new Event();	
		
		atualizado.setSeq(novo.getSeq());
		atualizado.setQsensos(novo.getQsensos() + velho.getQsensos());
		atualizado.setQasensos(novo.getQasensos() + velho.getQasensos());
		atualizado.setQntRel(novo.getQntRel() + velho.getQntRel());
		return atualizado;
	}
	
	public void printDB(){
		Set<String> s = dbMap.keySet();
		Iterator<String> iterator = s.iterator();
		
		while(iterator.hasNext()){
			String key = iterator.next();
			Event e = dbMap.get(key);
			System.out.println(e.getSeq()+" S:"+e.getQsensos()+" AS:"+e.getQasensos());
		}
	}
	
	public void close(){
		db.close();
	}

	public boolean getRegiaoCincoLSupport() {
		return regiaoCincoLSupport;
	}

	public void setRegiaoCincoLSupport(boolean regiaoCincoLSupport) {
		this.regiaoCincoLSupport = regiaoCincoLSupport;
	}
}

