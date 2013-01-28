package Hunt;

import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.Reader;
import java.io.StringWriter;
import java.io.Writer;
import java.lang.ProcessBuilder.Redirect;
import java.nio.charset.Charset;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.List;
import Gui.Drawer;

public class Hunter{
	
	private ProcessBuilder pb;
	private String shuntercmd_win = "SHUNTER";
	private String CUDA_ENV_WIN = "CUDA_BIN_PATH";
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
			String command = System.getenv("SHUNTER") +" "+ libsPath + " " + parameters;
					
			Drawer.writeToLog(command);
			
			// Instancia ProcessBuilder
			
			// On Linux/Mac
			//pb = new ProcessBuilder("bash","-c",command);
			
			// On Windows
			pb = new ProcessBuilder("cmd","/c",command);
			
			//pb.environment().put("LD_LIBRARY_PATH","/usr/local/cuda/lib64:/usr/local/cuda/lib");
			pb.redirectErrorStream(true);	
			//pb.redirectOutput(Redirect.PIPE);
			
			// Instancia interpretador
			t = new Translator(pb);
		
	}
	
	public void start(){
			t.start();
	}
	
	public void stop(){
		t.kill();
	}
	
}
