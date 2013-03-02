package hunt;

import gui.Drawer;

import java.lang.ProcessBuilder.Redirect;
import java.nio.charset.Charset;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.Map;


public class Hunter{

	private ProcessBuilder pb;
	private static String appName_win = "SHunter64.exe";
	private static String appName_mac = "";
	private static String appName_nix = "shunter-cli";
	String command = "";
	private Translator t;
	final static Charset ENCODING = StandardCharsets.UTF_8;
	private static String OS;

	public Hunter(String target,ArrayList<String> libs){	

		// Gera linha de parametros
		String parameters = new String("--target "+ target+ " -sdkt --gui");
		String libsPath = " ";
		if(!libs.isEmpty()){
			for(int i=0;i<libs.size();i++)
				libsPath = libsPath.concat("\""+libs.get(i))+"\" ";
		}
		setOS(System.getProperty("os.name").toUpperCase());

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
		pb.redirectOutput(Redirect.PIPE);

		// Instancia interpretador
		t = new Translator(pb);

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
		}else if (getOS().contains("NIX")){
			return appName_nix;			
		}else{
			return null;
		}
	}

	public static String getOS() {
		return OS;
	}

	public void setOS(String oS) {
		OS = oS;
	}

}
