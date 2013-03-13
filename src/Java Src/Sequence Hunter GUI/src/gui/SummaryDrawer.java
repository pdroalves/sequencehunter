package gui;

import hunt.Hunter;

import java.awt.BorderLayout;
import java.awt.Component;
import java.awt.Container;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;
import java.util.ArrayList;

import javax.swing.Box;
import javax.swing.JButton;
import javax.swing.JFileChooser;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextField;

public class SummaryDrawer implements ActionListener{
	private static String searchSeq;
	private static JLabel searchSeqJLabel;
	private static JTextField outputfolderTF;
	private static Hunter h;
	private static JButton startButton;
	private static JButton abortButton;
	private JPanel summaryContainer;
	private static Box libSummaryVBox;
	private Box outputFolderSummaryVBox;
	private JButton browseOutputButton;
	private static Drawer drawer;
	private static ArrayList<String> libs;
	
	public SummaryDrawer(Drawer d,Hunter hunterInstance){
		drawer = d;
		h = hunterInstance;
		libs = new ArrayList<String>();
		
		// Start cancel buttons
		startButton = new JButton("Start");
		startButton.addActionListener(this);
		abortButton = new JButton("Abort");
		abortButton.setEnabled(false);
		abortButton.addActionListener(this);
		
		// Instancia elementos do Panel
		searchSeqJLabel = new JLabel("");
		libSummaryVBox = Box.createVerticalBox();
		
		// Instancia output options
		outputFolderSummaryVBox = Box.createHorizontalBox();
		
		browseOutputButton = new JButton("Browse");
		browseOutputButton.addActionListener(this);
		browseOutputButton.setEnabled(false);
		
		outputfolderTF = new JTextField(50);
		outputfolderTF.setEditable(false);
		
		outputfolderTF.setText(h.getOutput());
		outputFolderSummaryVBox.add(outputfolderTF);
		outputFolderSummaryVBox.add(browseOutputButton);
		
		// Initial draw
		summaryContainer = new JPanel(new BorderLayout());
		drawSummaryContainer();
	}
	
	public JPanel getContainer(){
		return summaryContainer;
	}
	
	public static void setTargetSeq(String targetSeq){
		searchSeq = targetSeq;
		searchSeqJLabel.setText(targetSeq);
	}
	
	public static void addLoadedLib(String libpath){
		// Adiciona lib
		JLabel lib = new JLabel(libpath);
		lib.setAlignmentY(Component.TOP_ALIGNMENT);
		libSummaryVBox.add(lib);
		libs.add(libpath);
	}
	
	public static void removeLoadedLib(String libpath){
		// Remove lib

		// To-do
	}
	
	private void drawSummaryContainer(){
		JPanel jp = new JPanel();
		jp.setLayout(new GridBagLayout());
		GridBagConstraints c = new GridBagConstraints();
		
		// Line - Target Sequence
	    c.fill = GridBagConstraints.HORIZONTAL;
	    c.weighty = 0.15;
	    c.weightx = 0.3;
	    c.gridx = 0;
	    c.gridy = 0;
		jp.add(new JLabel("Target sequence: "),c);

		// Line - Target Sequence
	    c.fill = GridBagConstraints.HORIZONTAL;
	    c.weighty = 0.1;
	    c.weightx = 0.3;
	    c.gridx = 1;
	    c.gridy = 0;
		jp.add(searchSeqJLabel,c);

		// Line - Loaded libraries
	    c.fill = GridBagConstraints.HORIZONTAL;
	    c.weightx = 0.3;
	    c.gridx = 0;
	    c.gridy = 1;
		jp.add(new JLabel("Loaded libraries: "),c);

		// Line - Loaded libraries
	    c.fill = GridBagConstraints.BOTH;
	    c.weighty = 0.65;
	    c.weightx = 0.7;
	    c.gridx = 1;
	    c.gridy = 1;
		JScrollPane jscrp = new JScrollPane(libSummaryVBox);
		jp.add(jscrp,c);
		
		// Line - Output Folder
	    c.fill = GridBagConstraints.HORIZONTAL;
	    c.weighty = 0.05;
	    c.weightx = 0.3;
	    c.gridx = 0;
	    c.gridy = 2;
		jp.add(new JLabel("Output folder: "),c);

		// Line - Output Folder
	    c.fill = GridBagConstraints.HORIZONTAL;
	    c.weighty = 0.05;
	    c.weightx = 0.4;
	    c.gridx = 1;
	    c.gridy = 2;
		jp.add(outputFolderSummaryVBox,c);

		// Line - Start/Abort buttons
		Box hbox = Box.createHorizontalBox();
	    c.fill = GridBagConstraints.HORIZONTAL;
	    c.weighty = 0.1;
	    c.weightx = 0.3;
	    c.gridx = 1;
	    c.gridy = 3;		
	    hbox.add(startButton);
	    hbox.add(abortButton);
		jp.add(hbox,c);
		summaryContainer.add(jp,BorderLayout.CENTER);
		return;
	}
	
	static public void huntDone(String libDatabse,File logFile){
		if(libDatabse != null){
			try {
				Thread.sleep(1000);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			ReportDrawer.addReport(libDatabse,logFile);
			drawer.moveToReportTab();
			Drawer.writeToLog("Hunt done.");
			Drawer.writeToLog("Check Report tab for results...");
		}
		startButton.setEnabled(true);
		abortButton.setEnabled(false);
		Drawer.enableStatusJLabels(false);
	}

	static public void huntAbort(){
		h.stop();
		Drawer.writeToLog("Hunt aborted");
		startButton.setEnabled(true);
		abortButton.setEnabled(false);
		Drawer.enableStatusJLabels(false);
	}
	
	@Override
	public void actionPerformed(ActionEvent ae) {
		switch(ae.getActionCommand()){
		case "Start":
			// Monta reportContainer
			Drawer.writeToLog("Starting the hunt...");
			startButton.setEnabled(false);
			abortButton.setEnabled(true);
			drawer.initProgressBar(drawer.getTotalLibSize(libs));
			h.Set(searchSeq,libs);
			h.start();				
			//addReport("/home/pedro/Projetos/LNBIO/SH/Out/eGilboa/FriMar10210002013.sqlite",null);
			break;
		case "Abort":
			huntAbort();
			break;
		case "Browse":
			JFileChooser jfcBrowse = new JFileChooser();
			jfcBrowse.setCurrentDirectory(new File(h.getOutput()));
			jfcBrowse.setDialogTitle("Select output folder");
			jfcBrowse.setMultiSelectionEnabled(false);
			jfcBrowse.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
			jfcBrowse.setAcceptAllFileFilterUsed(false);
			if(jfcBrowse.showOpenDialog(null) == JFileChooser.APPROVE_OPTION){
				h.setOutput(jfcBrowse.getSelectedFile().getAbsolutePath());
				outputfolderTF.setText(h.getOutput());
			}
			break;
		}
		
	}
}
