package tables.report;

import gui.Drawer;

import java.io.File;
import java.io.FileNotFoundException;
import java.util.Scanner;

import javax.swing.JComponent;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;

import xml.TranslationsManager;

public class TextReport extends Report{

	private File txtFile;
	
	public TextReport(File txtFile){
		this.txtFile = txtFile;
	}
	public JComponent getReport(){
		final JTextArea logJTA = new JTextArea();
		TranslationsManager tm = TranslationsManager.getInstance();
		logJTA.setLineWrap(true);
		logJTA.setWrapStyleWord(true);
		try {
			Scanner scLog = new Scanner(txtFile);
			while(scLog.hasNextLine()){
				String linha = scLog.nextLine();
				logJTA.append(linha+"\n");
			}
			scLog.close();
			JScrollPane jscrlp = new JScrollPane(logJTA);
			return jscrlp;
		} catch (FileNotFoundException e1) {
			Drawer.writeToLog(txtFile.getAbsolutePath()+tm.getText("fileNotFound"));
			return null;
		}
	}
	
	public File getFile(){
		return txtFile;
	}
}
