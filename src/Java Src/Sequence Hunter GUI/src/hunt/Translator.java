package hunt;

import gui.Drawer;

import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import socket.ISocketUser;
import socket.SocketManager;


public class Translator extends Thread implements ISocketUser{

	private ProcessBuilder pb;
	private Process process;
	private boolean stop;
	private Pattern seqReadPattern;
	private Pattern outputPattern;
	private Pattern logPattern;
	private String outputDatabase;
	private String logFile;
	private SocketManager sm;

	public Translator(ProcessBuilder p){
		pb = p;
		stop = false;
		seqReadPattern = Pattern.compile("T(\\d+)S(\\d+)AS(\\d+)");
		outputPattern = Pattern.compile("DB (.*)");
		logPattern = Pattern.compile("Log (.*)");
	}

	public void run(){
		// Instancia e inicia o processo
		try {
			process = pb.start();
			sm = new SocketManager(this);
			sm.waitForConnections();
			
			if(!stop)
				if(outputDatabase != null)
					Drawer.huntDone(outputDatabase,new File(logFile));
				else
					Drawer.huntDone(null,null);
			killProcess(process);
		}catch(IllegalArgumentException e){
			e.printStackTrace();
			Drawer.writeToLog("Hunt fail.");
			Drawer.huntAbort();
		}catch (IOException e) {
			e.printStackTrace();
			Drawer.writeToLog("Hunt fail.");
			Drawer.huntAbort();
		}
		return;
	}
	
	public void translate(String s){
		Matcher matcher = seqReadPattern.matcher(s);
		if(matcher.find()){
			Drawer.setProcessedSeqs(Integer.parseInt(matcher.group(1)));
			Drawer.setSensosFounded(Integer.parseInt(matcher.group(2)));
			Drawer.setAntisensosFounded(Integer.parseInt(matcher.group(3)));
			Drawer.setSPS();
			System.out.println(s);
		}else{
			matcher = outputPattern.matcher(s);
			if(matcher.matches()){
				outputDatabase = matcher.group(1);
			}else{
				matcher =logPattern.matcher(s);
				if(matcher.matches()){
					logFile = matcher.group(1);
				}else{
					Drawer.writeToLog(s);	
				}
			}
		}
		return;
	}

	public void kill(){
		stop = true;
		return;
	}

	private void killProcess(Process p){
		String OS = Hunter.getOS();
		if (OS.contains("WIN")){
			//Windows
			String processData = "cmd /c tasklist /FI " + "\"" + "IMAGENAME eq " + Hunter.getAppName() + "\"" ;
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
			String tkProcess = "killall "+Hunter.getAppName()+" -q";
			try {
				Runtime.getRuntime().exec(tkProcess);
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
	}

}

