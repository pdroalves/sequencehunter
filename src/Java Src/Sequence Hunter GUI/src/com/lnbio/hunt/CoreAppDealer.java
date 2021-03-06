package com.lnbio.hunt;


import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import com.lnbio.gui.Drawer;
import com.lnbio.gui.SummaryDrawer;
import com.lnbio.gui.modules.SLStreamWriter;
import com.lnbio.socket.ISocketUser;
import com.lnbio.socket.SocketManager;
import com.lnbio.xml.TranslationsManager;



public class CoreAppDealer extends Thread implements ISocketUser{

	private ProcessBuilder pb;
	private Process process;
	private boolean stop;
	private Pattern seqReadPattern;
	private Pattern outputPattern;
	private Pattern logPattern;
	private String outputDatabase;
	private String logFile;
	private SocketManager sm;

	public CoreAppDealer(ProcessBuilder p){
		pb = p;
		stop = false;
		seqReadPattern = Pattern.compile("T(\\d+)S(\\d+)AS(\\d+)SPS(\\d+)BR([-,0-9]+)");
		outputPattern = Pattern.compile("DB (.*)");
		logPattern = Pattern.compile("Log (.*)");
	}

	public void run(){
		// Instancia e inicia o processo
		try {
			process = pb.start();
			Drawer.enableStatusJLabels(true);
			InputStream is = process.getInputStream();
			SLStreamWriter statusLogWriter = new SLStreamWriter(is);
			sm = new SocketManager(this);
			
			statusLogWriter.start();
			sm.waitForConnections();
			
			if(!stop){
				if(outputDatabase != null)
					SummaryDrawer.huntDone(outputDatabase,new File(logFile));
				else
					SummaryDrawer.huntDone(null,null);
			}else{
				killProcess(process);
				Drawer.writeToLog(TranslationsManager.getInstance().getText("statusHuntAbortMsg"));
				stop = false;
			}
		}catch(IllegalArgumentException e){
			e.printStackTrace();
			Drawer.writeToLog("Hunt fail.");
			SummaryDrawer.huntAbort();
		}catch (IOException e) {
			e.printStackTrace();
			Drawer.writeToLog("Hunt fail.");
			SummaryDrawer.huntAbort();
		}
		return;
	}
	
	public void translate(String s){
		Matcher matcher = seqReadPattern.matcher(s);
		if(matcher.find()){
			Drawer.setProcessedSeqs(Integer.parseInt(matcher.group(1)));
			Drawer.setSensosFounded(Integer.parseInt(matcher.group(2)));
			Drawer.setAntisensosFounded(Integer.parseInt(matcher.group(3)));
			Drawer.setSPS(Integer.parseInt(matcher.group(4)));
			long progress = Long.parseLong(matcher.group(5));
			Drawer.updateProgressBar(progress);
			System.err.println("Processing info.");
		}else{
			matcher = outputPattern.matcher(s);
			if(matcher.matches()){
				outputDatabase = matcher.group(1);
				System.err.println("Output file info.");
			}else{
				matcher =logPattern.matcher(s);
				if(matcher.matches()){
					logFile = matcher.group(1);
					System.err.println("Output log file info.");
				}else{
					Drawer.writeToLog(s);	
					System.err.println("Dunno.");
				}
			}
		}
		return;
	}

	public void kill(){
		stop = true;
		killProcess(process);
		return;
	}
	
	public boolean getKill(){
		return stop;
	}

	private void killProcess(Process p){
		String OS = Hunter.getOS();
		if (OS.contains("WIN")){
			//Windows
			String processData = "cmd /c tasklist /FI " + "\"" + "IMAGENAME eq " + Hunter.getFileAppName() + "\"" ;
			Pattern taskIdPattern = Pattern.compile("(\\d+)");
			try {
				Process process = Runtime.getRuntime().exec(processData);
				InputStream shellIn = process.getInputStream();
				BufferedReader br = new BufferedReader(new InputStreamReader(shellIn,"UTF-8"));
				while(!br.ready()){
					
				}
				String buffer = br.readLine();
				while(buffer != null){
					Matcher taskIdMatcher = taskIdPattern.matcher(buffer);
					if(taskIdMatcher.find()){
						String tkProcess = "cmd /c taskkill /PID "+taskIdMatcher.group(1)+" /F";
						Runtime.getRuntime().exec(tkProcess);
					}
					buffer = br.readLine();
				}
				shellIn.close();
				
				shellIn.close();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} 
		}else if (OS.contains("MAC")){
			// Mac
		}else if (OS.contains("NUX")){
			// Linux
			String tkProcess = "killall "+Hunter.getFileAppName();
			System.err.println(tkProcess);
			try {
				Runtime.getRuntime().exec(tkProcess);
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
	}

}

