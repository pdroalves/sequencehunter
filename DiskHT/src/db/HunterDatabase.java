package db;

import java.io.File;
import java.util.HashMap;
import java.util.Map;
import java.io.IOException;
import java.util.regex.Pattern;
import org.mapdb.*;


public class HunterDatabase {
	private HTreeMap<String, SeqEvento> db;
	private boolean isPrimary;

	public HunterDatabase(String dbName,File f){
		db = DBMaker.newFileDB(f).make().getHashMap(dbName);
	}
	
	public void addSeq(String key,SeqEvento value){
		SeqEvento returnSE = db.get(key);
		if(returnSE != null){
			db.put(key, updateValue(value,returnSE));
		}else{
			db.put(key, value);
		}
	}
	
	private SeqEvento updateValue(SeqEvento novo, SeqEvento velho){
		SeqEvento atualizado = new SeqEvento();	
		
		atualizado.setSeq(novo.getSeq());
		atualizado.setQsensos(novo.getQsensos() + velho.getQsensos());
		atualizado.setQantisensos(novo.getQantisensos() + velho.getQantisensos());
		atualizado.setQntRelativa(novo.getQntRelativa() + velho.getQntRelativa());
		return atualizado;
	}
	
	public void close(){
		db.close();
	}
}

