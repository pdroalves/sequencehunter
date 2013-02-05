package Hunt;

import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import Gui.Drawer;

public class Translator extends Thread{
	
	private ProcessBuilder pb;
	private Process process;
	private boolean stop;
	private Pattern seqReadPattern;
	private Pattern outputPattern;
	private String outputFile;
	
	public Translator(ProcessBuilder p){
		pb = p;
		stop = false;
		seqReadPattern = Pattern.compile("T(\\d+)S(\\d+)AS(\\d+)");
		outputPattern = Pattern.compile("Bin (.*)");
	}
	
	public void run(){
		// Instancia e inicia o processo
		try {
			process = pb.start();
			InputStream shellIn = process.getInputStream();
			read(shellIn);
			if(!stop)
				if(outputFile != null)
					Drawer.huntDone(new File(outputFile));
				else
					Drawer.huntDone(null);
			shellIn.close();
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
	
	public void read(InputStream is) throws IOException{
		if(is != null){
			
			BufferedReader br = new BufferedReader(new InputStreamReader(is,"UTF-8"));
			String buffer = br.readLine();
			while(buffer != null && !stop){
				translate(buffer);
				buffer = br.readLine();
			}
			is.close();
		}
		return;
	}
	
	private void translate(String s){
		Matcher matcher = seqReadPattern.matcher(s);
		if(matcher.matches()){
			Drawer.setProcessedSeqs(Integer.parseInt(matcher.group(1)));
			Drawer.setSensosFounded(Integer.parseInt(matcher.group(2)));
			Drawer.setAntisensosFounded(Integer.parseInt(matcher.group(3)));
			Drawer.setSPS();
			System.out.println(s);
		}else{
			matcher = outputPattern.matcher(s);
			if(matcher.matches()){
				outputFile = matcher.group(1);
			}else{
				Drawer.writeToLog(s);	
			}
		}
		return;
	}
	
	public void kill(){
		stop = true;
		return;
	}
	
	public void killProcess(Process p){
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
	}
}
