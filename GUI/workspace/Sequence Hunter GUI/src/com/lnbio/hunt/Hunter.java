package com.lnbio.hunt;


import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.nio.charset.Charset;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import com.lnbio.gui.Drawer;
import com.lnbio.xml.TranslationsManager;



public class Hunter{

	private ProcessBuilder pb;
	private final static String appName = "Sequence Hunter";
	private static String appFileName_win = "%SHUNTER%";
	private static String appFileName_mac = "";
	private static String appFileName_nix = "shunter";
	private static String default_output_folder;
	private static String output_folder;
	public static final int FORCE_CUDA_MODE = 0;
	public static final int FORCE_NONCUDA_MODE = 1;
	public static final int NON_FORCE_MODE = 2;
	private int mode = NON_FORCE_MODE;
	private boolean storeCLinhaSequences;
	private boolean storeFullSequence;
	private long tamCLinha;
	private long distCLinha;
	public static Hunter instance;
	private String targetSequence;

	String command = "";
	private CoreAppDealer t;
	final static Charset ENCODING = StandardCharsets.UTF_8;
	private TranslationsManager tm;
	
	private Hunter(){
		setCLinhaStorage(false);
		setFullSequenceStorage(false);
		setTamCLinha(0);
		setDistCLinha(0);
		setMode(NON_FORCE_MODE);
		tm = TranslationsManager.getInstance();
		if(getOS().contains("WIN")){
			default_output_folder = new String(System.getenv("HOMEDRIVE")+System.getenv("HOMEPATH"));
		}else if(getOS().contains("MAC")){

		}else if(getOS().contains("NUX")){
			default_output_folder = System.getenv("HOME");
		}
	}
	
	public static Hunter getInstance(){
		if(instance == null){
			instance = new Hunter();
		}
		
		return instance;
	}

	public void setOutput(String output){
		output_folder = output;
	}

	public String getOutput(){
		if(output_folder != null){
			return output_folder;
		}else{
			return default_output_folder;
		}
	}

	public void Set(String target,ArrayList<String> libs){	

		// Gera linha de parametros
		setTargetSequence(target);
		String parameters = new String("--target "+getTargetSequence()+ " --gui ");

		switch(mode){
		case FORCE_CUDA_MODE:
			parameters = parameters.concat("-e ");
			break;
		case FORCE_NONCUDA_MODE:
			parameters = parameters.concat("-d ");
			break;
		default:
			break;
		}
		
		if(output_folder != null){
			parameters = parameters.concat("-o \""+output_folder+"\" ");
		}
		
		if(!isFullSequenceStored()){
			parameters = parameters.concat("-t ");
		}
		
		if(isCLinhaStored()){
			parameters = parameters.concat(" --dist5l="+getDistCLinha()+" --tam5l="+getTamCLinha());
		}
		
		String libsPath = " ";
		if(!libs.isEmpty()){
			for(int i=0;i<libs.size();i++)
				libsPath = libsPath.concat("\""+libs.get(i))+"\" ";
		}

		if (getOS().contains("WIN")){
			//Windows
			command = appFileName_win +" "+ libsPath + " " + parameters;
			pb = new ProcessBuilder("cmd","/c",command);
		}else if (getOS().contains("MAC")){
			// Mac
		}else if (getOS().contains("NUX")){
			// Linux
			command = appFileName_nix +" "+ libsPath + " " + parameters;

			// On Linux/Mac
			// Instancia ProcessBuilder
			pb = new ProcessBuilder("bash","-c",command);
			String path = System.getenv("PATH");
			pb.environment().put("PATH",path);
			pb.environment().put("LD_LIBRARY_PATH", "/usr/local/cuda/lib64:/usr/local/cuda/lib");
		}

		Drawer.writeToLog(command);
		pb.redirectErrorStream(true);	

		// Instancia interpretador
		t = new CoreAppDealer(pb);

	}

	public void start(){
		t.start();
	}

	public void stop(){
		t.kill();
	}

	public int getCLIBuild(){
		int build = -1;
		String command = null;

		if (getOS().contains("WIN")){
			//Windows
			command = appFileName_win +" -b";
			pb = new ProcessBuilder("cmd","/c",command);
		}else if (getOS().contains("MAC")){
			// Mac
		}else if (getOS().contains("NUX")){
			// Linux
			command = appFileName_nix +" -b";

			// On Linux/Mac
			// Instancia ProcessBuilder
			pb = new ProcessBuilder("bash","-c",command);
			String path = System.getenv("PATH");
			pb.environment().put("PATH",path);
			pb.environment().put("LD_LIBRARY_PATH", "/usr/local/cuda/lib64:/usr/local/cuda/lib");
		}

		try {
			Process process = pb.start();
			InputStream is = process.getInputStream();
			BufferedReader reader = new BufferedReader(new InputStreamReader(is));
			String msg = reader.readLine();
			Pattern pattern = Pattern.compile("^[B][u][i][l][d][:]\\s([0-9])$");
			Matcher matcher = pattern.matcher(msg);
			if(matcher.find()){
				build = Integer.parseInt(matcher.group(1));
			}
		} catch (IOException e) {
			Drawer.writeToLog(tm.getText("NoCLIConnection") + " "+e.getMessage());
		}

		return build;
	}
	static public String getFileAppName(){
		if (getOS().contains("WIN")){
			return appFileName_win;			
		}else if (getOS().contains("MAC")){
			return appFileName_mac;
		}else if (getOS().contains("NUX")){
			return appFileName_nix;			
		}else{
			return null;
		}
	}

	static  public String getAppName(){
		return appName + " "+Hunter.getVersion();
	}

	static public String getVersion(){
		String command = "";
		String build = "-1";
		ProcessBuilder pb = null;
		if (getOS().contains("WIN")){
			//Windows
			command = appFileName_win +" -b";
			pb = new ProcessBuilder("cmd","/c",command);
		}else if (getOS().contains("MAC")){
			// Mac
		}else if (getOS().contains("NUX")){
			// Linux
			command = appFileName_nix +" -b";

			// On Linux/Mac
			// Instancia ProcessBuilder
			pb = new ProcessBuilder("bash","-c",command);
			String path = System.getenv("PATH");
			pb.environment().put("PATH",path);
			pb.environment().put("LD_LIBRARY_PATH", "/usr/local/cuda/lib64:/usr/local/cuda/lib");
		}

		try {
			Process process = pb.start();
			BufferedReader reader = new BufferedReader(new InputStreamReader(process.getInputStream()));
			build = reader.readLine();
		} catch (IOException e) {
			Drawer.writeToLog(TranslationsManager.getInstance().getText("NoCLIConnection"));
		}
		return build;
	}

	public static String getOS() {
		return System.getProperty("os.name").toUpperCase();
	}

	public int getMode() {
		return mode;
	}

	public void setMode(int mode) {
		this.mode = mode;
	}

	public boolean isCLinhaStored() {
		return storeCLinhaSequences;
	}

	public void setCLinhaStorage(boolean storeCLinhaSequences) {
		this.storeCLinhaSequences = storeCLinhaSequences;
	}

	public boolean isFullSequenceStored() {
		return storeFullSequence;
	}

	public void setFullSequenceStorage(boolean storeFullSequence) {
		this.storeFullSequence = storeFullSequence;
	}

	public long getTamCLinha() {
		return tamCLinha;
	}

	public void setTamCLinha(long n) {
		this.tamCLinha = n;
	}

	public long getDistCLinha() {
		return distCLinha;
	}

	public void setDistCLinha(long distCLinha) {
		this.distCLinha = distCLinha;
	}

	public String getTargetSequence() {
		return this.targetSequence;
	}

	public void setTargetSequence(String targetSequence) {
		this.targetSequence = targetSequence;
	}
}
