package main;

import java.io.File;
import db.HunterDatabase;
import db.Event;

public class Main {
	public static void main(String[] args) {
		System.out.println("Starting...");
			File f = new File("data");
			HunterDatabase hdb = new HunterDatabase("data", f);
			hdb.add("Teste", new Event("Teste",5,1));
			hdb.printDB();
			hdb.commit();
			hdb.close();
		System.out.println("Done");	
	}

}
