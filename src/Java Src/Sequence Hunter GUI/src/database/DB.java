package database;

import java.io.File;

public class DB {
	String filename;
	File fp;
	
	public DB(String fn){
		filename = fn;
		fp = new File(fn);
	}

	public String getFilename(){
		return fp.getName();
	}
	
}
