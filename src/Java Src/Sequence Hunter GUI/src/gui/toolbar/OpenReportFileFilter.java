package gui.toolbar;

import java.io.File;

import javax.swing.filechooser.FileFilter;

public class OpenReportFileFilter extends FileFilter{

	@Override
	public boolean accept(File f) {
		if(f.getName().endsWith(".db")){
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
		return "*.db";
	}

}
