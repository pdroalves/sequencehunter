package tables;

import hunt.Evento;
import java.util.ArrayList;
import database.DBManager;


public class JReportLoadData{
	private DBManager dbm;
	private int defaultLoad;
	private ArrayList<Evento> seqs;

	public JReportLoadData(int dl,ArrayList<Evento> al,DBManager dbm){
		this.dbm = dbm;
		dbm.loadSort();
		seqs = al;
		defaultLoad = dl;

		for(int i = 0; i < defaultLoad;i++){
			Evento e = this.dbm.getEvento();
			if(e != null){
				seqs.add(e);
			}else{
				return;
			}
		}
	}

	public void load(){
		Evento e = dbm.getEvento();
		if(e != null) 
			seqs.add(e);
		else 
			return;
	}

}
