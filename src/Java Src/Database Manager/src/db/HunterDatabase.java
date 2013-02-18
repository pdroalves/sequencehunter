//      SocketManager.java
//      
//      Copyright 2013 Pedro Alves <pdroalves@gmail.com>
//      
//		Classe que implementa MapDB para armazenamento e leitura de dados do banco de dados com objetos do tipo Event.
//		Serve de auxilio para o Sequence Hunter.
//
//		08/02/2013

package db;

import java.io.DataInput;
import java.io.DataOutput;
import java.io.File;
import java.io.IOException;
import java.io.Serializable;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.Set;
import org.mapdb.*;


public class HunterDatabase implements Serializable{
	private DB db;
	private Map<String, Event> dbMap;
	
	public HunterDatabase(String dbName,File f){
		db = DBMaker.newFileDB(f)
				.asyncThreadDaemonEnable()
				.cacheLRUEnable()
				.make();
		dbMap = db.getHashMap(dbName);
	}
	
	public void add(String keyCentral,String keyCL,Event value){
		// A sequencia deve vir jah segmentada
		Event returnSE = dbMap.get(keyCentral);
		if(returnSE != null){
			dbMap.put(keyCentral, updateValue(value,returnSE,keyCL));
		}else{
			if(keyCL != null){
				value.setDbCincoL(new HashMap<String,Event>());
				value.getDbCincoL().put(keyCL, value);
			}
			dbMap.put(keyCentral, value);
		}
	}
	
	public void removeAll(){
		dbMap.clear();
	}
	
	public void commit(){
		long start = System.currentTimeMillis();
		db.commit();
		long elapsedTime = System.currentTimeMillis() - start;
		System.out.println("Time to commit: "+elapsedTime+" ms");
	}
	
	public void compact(){
		this.commit();
		db.compact();
	}
	
	private Event updateValue(Event novo, Event velho,String keyCL){
		Event atualizado = new Event();	
		
		atualizado.setQsensos(novo.getQsensos() + velho.getQsensos());
		atualizado.setQasensos(novo.getQasensos() + velho.getQasensos());
		atualizado.setQntRel(novo.getQntRel() + velho.getQntRel());
		atualizado.setDbCincoL(velho.getDbCincoL());
		if(keyCL != null){
			atualizado.setDbCincoL();
			Event cincoL = atualizado.getDbCincoL().get(keyCL);
			if(cincoL == null){
				cincoL = new Event(novo.getQsensos(),novo.getQasensos(),novo.getQntRel());
				atualizado.getDbCincoL().put(keyCL,cincoL);
			}else{
				cincoL.setQsensos(cincoL.getQsensos()+novo.getQsensos());
				cincoL.setQasensos(cincoL.getQasensos()+novo.getQasensos());
				cincoL.setQntRel(cincoL.getQntRel()+novo.getQntRel());
			}
		}
		return atualizado;
	}
	
	public void printDB(){
		Set<String> s = dbMap.keySet();
		Iterator<String> iterator = s.iterator();
		
		while(iterator.hasNext()){
			String key = iterator.next();
			Event e = dbMap.get(key);
			System.out.println(key+" S:"+e.getQsensos()+" AS:"+e.getQasensos());
			e.printDB();
		}
	}
	
	public void close(){
		db.close();
	}
	
	public int size(){
		return dbMap.size();
	}

	public Set<String> keySet(){
		return dbMap.keySet();
	}
	
	/*Serializer<Event> serializer = new Serializer<Event>(){

		@Override
		public void serialize(DataOutput out, Event value) throws IOException {
			out.writeUTF(value.getSeq());
			out.write(value.getQsensos());
			out.write(value.getQasensos());
			out.writeDouble(value.getQntRel());
			Map<String,Event> dbLocal = value.getDbCincoL();
			out.write
		}

		@Override
		public Event deserialize(DataInput in, int available)
				throws IOException {
			// TODO Auto-generated method stub
			return null;
		}
		
	};*/

}


