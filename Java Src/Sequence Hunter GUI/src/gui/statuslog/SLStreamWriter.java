package gui.statuslog;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;

import gui.Drawer;

public class SLStreamWriter extends Thread{
	// Monitora um stream para escrita no status log
	
	private InputStream is;
	
	public SLStreamWriter(InputStream is){
		this.is = is;
	}

	@Override
	public void run() {
		BufferedReader reader = new BufferedReader(new InputStreamReader(is));
		
		String msg;
		try {
			while((msg = reader.readLine()) != null){
				Drawer.writeToLog(msg);
			}
		} catch (IOException e) {
			Drawer.writeToLog(e.getMessage());
		}
		
	}
	
	
}
