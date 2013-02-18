package main;

import java.io.File;

import socket.SocketManager;
import db.HunterDatabase;
import db.Event;

public class Main {
	public static void main(String[] args) {
		int porta;
		if(args.length > 0)
			porta = Integer.parseInt(args[0]);
		else
			porta = 9332;
		System.out.println("Starting...\nPort: "+porta);
		HunterDatabase db = new HunterDatabase("central", new File("centralSeq"));
		SocketManager sm = new SocketManager(db);
		sm.waitForConnections();
		//db.removeAll();ls
		
		db.printDB();
		db.commit();
		db.close();
		
	}

}
