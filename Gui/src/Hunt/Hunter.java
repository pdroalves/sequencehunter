package Hunt;

import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.Reader;
import java.io.StringWriter;
import java.io.Writer;
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
	private String shuntercmd = "/home/pedro/Projetos/bin/shunter-cmd";
	private Translator t;
	final static Charset ENCODING = StandardCharsets.UTF_8;

	public Hunter(List<String> target,ArrayList<String> libs){
	    try {
			// Gera shset.dat
		    Path path = Paths.get("shset.dat");
			Files.write(path, target, ENCODING);
			
			// Gera linha de parametros
			String parameters = new String("-fsd --gui");
			String libsPath = " ";
			if(!libs.isEmpty()){
				for(int i=0;i<libs.size();i++)
					libsPath = libsPath.concat(" " + libs.get(i));
			}
			String command = shuntercmd +" "+ libsPath +" "+ parameters;
			Drawer.writeToLog(command);
			
			// Instancia ProcessBuilder
			pb = new ProcessBuilder("bash","-c",command);
			pb.environment().put("LD_LIBRARY_PATH","/usr/local/cuda/lib64:/usr/local/cuda/lib");
			pb.redirectErrorStream(true);			
			
			// Instancia interpretador
			t = new Translator(pb);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	public void start(){
			t.start();
	}
	
	public void stop(){
		t.kill();
	}
	
}
