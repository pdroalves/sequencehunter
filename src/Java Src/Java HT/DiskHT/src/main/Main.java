package main;

import java.io.File;

import socket.SocketManager;
import db.HunterDatabase;
import db.Event;

public class Main {
	public static void main(String[] args) {
		System.out.println("Starting...");
		HunterDatabase db = new HunterDatabase("central", new File("centralSeq"));
		SocketManager sm = new SocketManager(db);
		sm.waitForConnections();
		//db.removeAll();
		db.printDB();
		db.commit();
		System.out.println("Done");	
	}

}
