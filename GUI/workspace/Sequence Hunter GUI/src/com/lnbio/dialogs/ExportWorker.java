package com.lnbio.dialogs;


import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.Calendar;
import java.util.Iterator;
import java.util.List;
import java.util.zip.ZipEntry;
import java.util.zip.ZipOutputStream;

import javax.swing.JFileChooser;
import javax.swing.JProgressBar;
import javax.swing.JTree;

import com.lnbio.auxiliares.checkbox.CheckBoxNode;
import com.lnbio.database.DBManager;
import com.lnbio.gui.Drawer;
import com.lnbio.hunt.Evento;
import com.lnbio.tables.report.JFiveCutQueryReportTableModel;
import com.lnbio.xml.TranslationsManager;


public class ExportWorker extends Thread {

	private JProgressBar jpb;
	private List<CheckBoxNode> cbnList;
	private long maxSeqsToExport;
	private JTree tree;
	private ExportDialog owner;

	public ExportWorker(ExportDialog owner,JProgressBar jpb,List<CheckBoxNode> cbnList,JTree tree,long maxSeqsToExport){
		this.jpb = jpb;
		this.cbnList = cbnList;
		this.maxSeqsToExport = maxSeqsToExport;
		this.tree = tree;
		this.owner = owner;
	}
	public void run(){
		JFileChooser jfc = new JFileChooser(System.getProperty("user.dir"));
		jfc.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
		if(jfc.showSaveDialog(null) == JFileChooser.APPROVE_OPTION){
			jpb.setVisible(true);
			Iterator<CheckBoxNode> iterator = cbnList.iterator();
			try {
				Calendar calendar = Calendar.getInstance();
				File zipfile = new File(jfc.getSelectedFile()+System.getProperty("file.separator")+"Export-"+calendar.getTime()+".zip");
				zipfile.createNewFile();
				ZipOutputStream out = new ZipOutputStream(new FileOutputStream(zipfile));
				while(iterator.hasNext()){
					CheckBoxNode cbn = iterator.next();
					if(cbn.isSelected() || !tree.isEnabled()){
						Object attribute = cbn.getAttribute();

						if(attribute instanceof DBManager){
							DBManager dbm = (DBManager) attribute;
							jpb.setVisible(true);
							jpb.setIndeterminate(true);
							List<Evento> eventos = null;
							if(cbn.getText().contains("Central")){
								dbm.centralCutLoad(maxSeqsToExport);
								eventos = dbm.getCentralCutEvents();
							}else if(cbn.getText().contains("Five")){
								dbm.fiveCutLoad(maxSeqsToExport);
								eventos = dbm.getFiveCutEvents();
							}else{
								Drawer.writeToLog(TranslationsManager.getInstance().getText("statusErrorProcessingMsg"));
								System.err.println("Error: Don't know what to do with report "+cbn.getText());
								owner.dispose();
								return;
							}
							jpb.setIndeterminate(false);
							jpb.setMaximum(eventos.size());
							jpb.setValue(0);

							// name the file inside the zip  file
							out.putNextEntry(new ZipEntry(cbn.getText()+".csv")); 
							Iterator<Evento> eventoIterator = eventos.iterator();

							// Imprime header
							String str;
							if(cbn.getText().contains("unpaired")){
								str = TranslationsManager.getInstance().getText("unpairedCSVHeader")+System.getProperty("line.separator"); 
							}else if(cbn.getText().contains("paired")){
								str = TranslationsManager.getInstance().getText("pairedCSVHeader")+System.getProperty("line.separator");
							}else{
								str = "";
							}
							out.write(str.getBytes("UTF-8"), 0, str.length());
							jpb.setValue(jpb.getValue() + 1);

							while(eventoIterator.hasNext()){
								Evento e = eventoIterator.next();
								if(cbn.getText().contains("unpaired")){
									str = e.getSeq()+","+e.getPares()+","+e.getSensos()+","+e.getAntisensos()+","+e.getRelativeFreq()+System.getProperty("line.separator"); 
								}else if(cbn.getText().contains("paired")){
									str = e.getSeq()+","+e.getPares()+","+e.getRelativeFreq()+System.getProperty("line.separator"); 				
								}else{
									str = "";
								}
								out.write(str.getBytes("UTF-8"), 0, str.length());
								jpb.setValue(jpb.getValue() + 1);
							}

						}else if(attribute instanceof File){
							File f = (File) attribute;
							FileInputStream in = new FileInputStream(f);
							// output file 
							out.putNextEntry(new ZipEntry(f.getName())); 

							byte[] b = new byte[1024];

							int count;

							while ((count = in.read(b)) > 0) {
								out.write(b, 0, count);
							}
							in.close();

						}else if(attribute instanceof JFiveCutQueryReportTableModel){
							JFiveCutQueryReportTableModel model = (JFiveCutQueryReportTableModel) attribute;
							List<Evento> eventos = null;
							jpb.setVisible(true);
							jpb.setIndeterminate(true);
							model.customHuntLoad(maxSeqsToExport);
							eventos = ((JFiveCutQueryReportTableModel) attribute).getData();

							jpb.setIndeterminate(false);
							jpb.setMaximum(eventos.size());
							jpb.setValue(0);

							// name the file inside the zip  file
							out.putNextEntry(new ZipEntry(cbn.getText()+".csv")); 
							Iterator<Evento> eventoIterator = eventos.iterator();

							// Imprime header
							String 	str = TranslationsManager.getInstance().getText("customCSVHeader")+System.getProperty("line.separator");
							out.write(str.getBytes("UTF-8"), 0, str.length());
							jpb.setValue(jpb.getValue() + 1);

							while(eventoIterator.hasNext()){
								Evento e = eventoIterator.next();
								str = e.getSeq()+","+e.getSensos()+System.getProperty("line.separator"); 
								out.write(str.getBytes("UTF-8"), 0, str.length());
								jpb.setValue(jpb.getValue() + 1);
							}

						}
					}
				}
				out.close();
			}catch (IOException e1) {
				Drawer.writeToLog(e1.getMessage());
			}
			jpb.setVisible(false);
			jpb.setValue(0);
			owner.setExported(true);
		}
	}
}
