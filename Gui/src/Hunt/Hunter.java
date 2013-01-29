package Hunt;

import java.lang.ProcessBuilder.Redirect;
import java.nio.charset.Charset;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import Gui.Drawer;

public class Hunter{
	
	private ProcessBuilder pb;
	private static String appName = "SHunter.exe";
	private static String shuntercmd = "%SHUNTER%";
	//private static String CUDA_ENV_WIN = "CUDA_BIN_PATH";
	private Translator t;
	final static Charset ENCODING = StandardCharsets.UTF_8;

	public Hunter(String target,ArrayList<String> libs){	
	  
			// Gera linha de parametros
			String parameters = new String("--target "+ target+ " -sdt --gui");
			String libsPath = " ";
			if(!libs.isEmpty()){
				for(int i=0;i<libs.size();i++)
					libsPath = libsPath.concat("\""+libs.get(i))+"\" ";
			}
			
			// Aqui falta um throw exception caso realshuntercmd seja nulo
			String command = shuntercmd +" "+ libsPath + " " + parameters;
					
			Drawer.writeToLog(command);
			
			// Instancia ProcessBuilder
			
			// On Linux/Mac
			//pb = new ProcessBuilder("bash","-c",command);
			
			// On Windows
			pb = new ProcessBuilder("cmd","/c",command);
			
			//pb.environment().put("LD_LIBRARY_PATH","/usr/local/cuda/lib64:/usr/local/cuda/lib");
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
		return appName;
	}
	
}
