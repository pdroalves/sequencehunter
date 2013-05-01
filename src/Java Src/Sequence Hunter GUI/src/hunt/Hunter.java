package hunt;

import gui.Drawer;

import java.nio.charset.Charset;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;


public class Hunter{

	private ProcessBuilder pb;
	private static String appName_win = "%SHUNTER%";
	private static String appName_mac = "";
	private static String appName_nix = "shunter-cli";
	private static String default_output_folder;
	private static String output_folder;
	String command = "";
	private CoreAppDealer t;
	final static Charset ENCODING = StandardCharsets.UTF_8;
	
	public Hunter(){
		if(getOS().contains("WIN")){
			default_output_folder = new String(System.getenv("HOMEDRIVE")+System.getenv("HOMEPATH"));
		}else if(getOS().contains("MAC")){
			
		}else if(getOS().contains("NUX")){
			default_output_folder = System.getenv("HOME");
		}
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
		String parameters = new String("--target "+ target+ " -dt --gui");
		if(output_folder != null){
			parameters.concat(" -o "+output_folder);
		}
		String libsPath = " ";
		if(!libs.isEmpty()){
			for(int i=0;i<libs.size();i++)
				libsPath = libsPath.concat("\""+libs.get(i))+"\" ";
		}

		if (getOS().contains("WIN")){
			//Windows
			command = appName_win +" "+ libsPath + " " + parameters;
			pb = new ProcessBuilder("cmd","/c",command);
		}else if (getOS().contains("MAC")){
			// Mac
		}else if (getOS().contains("NUX")){
			// Linux
			command = appName_nix +" "+ libsPath + " " + parameters;

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

	static public String getAppName(){
		if (getOS().contains("WIN")){
			return appName_win;			
		}else if (getOS().contains("MAC")){
			return appName_mac;
		}else if (getOS().contains("NUX")){
			return appName_nix;			
		}else{
			return null;
		}
	}

	public static String getOS() {
		return System.getProperty("os.name").toUpperCase();
	}
}
