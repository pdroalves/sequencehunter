package auxiliares;

import java.io.File;

import javax.swing.filechooser.FileFilter;

public class JTxtFileFilter extends FileFilter{

	@Override
	public boolean accept(File f) {
		if(f.getName().endsWith(".txt")||
				f.getName().endsWith(".dat")){
			return true;
		}else{
			if(f.isDirectory()){
				return true;
			}
		}
		return false;
	}

	@Override
	public String getDescription() {
		return "*.txt,*.dat";
	}

}
