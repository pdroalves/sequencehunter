package gui;
import hunt.Hunter;
import hunt.Library;

import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.AdjustmentEvent;
import java.awt.event.AdjustmentListener;
import java.io.File;
import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Iterator;
import java.util.List;
import java.util.Scanner;

import javax.swing.*;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;
import javax.swing.table.DefaultTableCellRenderer;

import tables.JLazyTableModel;
import tables.JReportTableModel;
import tables.JTableRenderer;
import tables.SelectionListener;

import dialogs.AboutDialog;

import auxiliares.JBaseTextField;
import auxiliares.JTxtFileFilter;


public class Drawer implements ActionListener {
	
	private JFrame jfrm;
	private static JTabbedPane jtp;
	private JBaseTextField seqOriginal;
	private String searchSeq;
	private static JLabel seqBusca;
	private JButton setSeqButton;
	private static JTextArea statusLog;
	private static JButton startButton;
	private static JButton abortButton;
	private JProgressBar jprog;
	private JPanel jpTableList;
	private JList<String> jl ;
	private DefaultListModel<String> listModel;
	private ArrayList<String> libs = new ArrayList<String>();
	private JTabbedPane libContainer;
	private JPanel summaryContainer;
	private static Container reportContainer;
	private static JTabbedPane reportTab;
	private static Boolean noReports = true;
	private int xSize = 700;
	private int ySize = 1000;
	private static Hunter h;
	private static JLabel processedSeqs;
	private static JLabel sensosFounded;
	private static JLabel antisensosFounded;
	private static JLabel calcSPS;
	private static long startSPS = -1;
	private static long diffSPS;
	
	public Drawer(){
		seqOriginal = new JBaseTextField(25);
		seqBusca = new JLabel();
		statusLog = new JTextArea();
		statusLog.setLineWrap(true);
		statusLog.setWrapStyleWord(true); 
		jl = new JList<String>();
		listModel = new DefaultListModel<String>();  
		libContainer = new JTabbedPane(JTabbedPane.TOP,JTabbedPane.SCROLL_TAB_LAYOUT);	
		libContainer.setPreferredSize(new Dimension(900,300));
		summaryContainer = new JPanel(new BorderLayout());
		reportContainer = drawEmptyReportContainer();
		addReport("/home/pedro/database.db",null);
		processedSeqs = new JLabel("");
		sensosFounded = new JLabel("");
		antisensosFounded = new JLabel("");
		calcSPS = new JLabel("");
		jpTableList = new JPanel();
		drawEmptyLibsContainer();
		
		// Start cancel buttons
		startButton = new JButton("Start");
		startButton.addActionListener(this);
		abortButton = new JButton("Abort");
		abortButton.setEnabled(false);
		abortButton.addActionListener(this);
		
		// Cria JFrame container
		jfrm = new JFrame("Sequence Hunter");
		jfrm.setResizable(false);
		
		// Seta FlowLayout para o content pane
		jfrm.getContentPane().setLayout(new BorderLayout());
				
		jfrm.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		jfrm.setSize(ySize,xSize);
		jfrm.setLocationByPlatform(true);
		jfrm.setLocationRelativeTo(null);
		
		//Gera menu
		jfrm.setJMenuBar(drawMenuBar());
		
		//Seta posicao inicial para centro da tela
		//jfrm.setLocationRelativeTo(null);	
		// Cria tabbed pane
		jtp = new JTabbedPane(JTabbedPane.TOP,JTabbedPane.SCROLL_TAB_LAYOUT);	
		
		// Monta searchContainer
		jtp.addTab("Setup",null,drawSearchContainer(),"Set what you want to search");
		
		// Monta summaryContainer
		jtp.addTab("Summary",null,drawSummaryContainer(),"Confirm the configuration and start the hunt");
		
		// Monta reportContainer
		jtp.addTab("Report",null,reportContainer,"Check the results after a hunt");

		
		JPanel top = new JPanel(new BorderLayout());
		top.add(jtp,BorderLayout.CENTER);
		// Monta statusContainer
		JPanel bottom = new JPanel(new BorderLayout());
		bottom.add(drawStatusContainer());		
		
		// Cria JSplitPane e adiciona JScrollpane nele
		top.setMinimumSize(new Dimension(0,0));
		bottom.setMinimumSize(new Dimension(0,0));
		JSplitPane jsp = new JSplitPane(JSplitPane.VERTICAL_SPLIT,true,top,bottom);
		//jsp.setDividerSize(10);
		jsp.setOneTouchExpandable(true);
				
		jfrm.add(jsp,BorderLayout.CENTER);
				
		jfrm.setVisible(true);
		
		jsp.setDividerLocation(0.70);
		jsp.setResizeWeight(0.5);
		jsp.setMaximumSize(new Dimension(xSize/3,ySize));
	}
	
	private JMenuBar drawMenuBar(){
		// Barra do menu
		JMenuBar menuBar = new JMenuBar();
		
		// Novo Menu  
		JMenu menuFile = new JMenu("File"); 
		JMenu menuHelp = new JMenu("Help");   
		
		// Item do menu  
		JMenuItem menuItemExit = new JMenuItem("Exit");  		
		JMenuItem menuItemAbout = new JMenuItem("About");		
		
		menuItemAbout.addActionListener(new ActionListener() {
			
			@Override
			public void actionPerformed(ActionEvent arg0) {
				JDialog about = new AboutDialog(jfrm);
				about.setVisible(true);
			}
		});
		
		menuFile.add(menuItemExit);
		menuHelp.add(menuItemAbout);
		menuBar.add(menuFile); 
		menuBar.add(menuHelp);
		
		return menuBar;
	}
	
	private Container drawSearchContainer(){				
		// Cria panels
		JPanel seqBuscaPanel = new JPanel();
		JPanel libs = new JPanel();
		
		// Configura tab para sequencias
		seqBuscaPanel.setLayout(new GridLayout(2,1));
		
		Box hbox = Box.createHorizontalBox();
		setSeqButton = new JButton("Set");
		setSeqButton.addActionListener(this);
		
		hbox.add(new JLabel("Sequence: "));
		hbox.add(seqOriginal);
		hbox.add(setSeqButton);
		seqBuscaPanel.add(hbox);
		hbox = Box.createHorizontalBox();
		hbox.add(new JLabel("Target Sequence: "));
		hbox.add(seqBusca);
		seqBuscaPanel.add(hbox);		
		
		JButton loadLib = new JButton("Load");
		JButton unloadLib = new JButton("Unload");
		loadLib.addActionListener(this);
		unloadLib.addActionListener(this);

		libs.setLayout(new GridLayout(2,2));
		libs.add(new JLabel("Libraries loaded: "));
		JScrollPane jscrlp = new JScrollPane(jl);
		jl.setModel(listModel);
		
		libs.add(jscrlp);
		
		hbox = Box.createHorizontalBox();
		hbox.add(loadLib);
		hbox.add(unloadLib);
		
		libs.add(hbox);

		// Adiciona tabs
		Box vbox = Box.createVerticalBox();
		vbox.add(seqBuscaPanel);
		vbox.add(libs);
		vbox.add(libContainer);
		
		return vbox;
	}
	
	private Container drawSummaryContainer(){
		JPanel jp = new JPanel();
		jp.setLayout(new GridBagLayout());
		GridBagConstraints c = new GridBagConstraints();
		
	    c.fill = GridBagConstraints.HORIZONTAL;
	    c.weighty = 0.15;
	    c.weightx = 0.3;
	    c.gridx = 0;
	    c.gridy = 0;
		jp.add(new JLabel("Target sequence: "),c);

	    c.fill = GridBagConstraints.HORIZONTAL;
	    c.weighty = 0.1;
	    c.weightx = 0.3;
	    c.gridx = 1;
	    c.gridy = 0;
		jp.add(new JLabel(searchSeq),c);
		
	    c.fill = GridBagConstraints.HORIZONTAL;
	    c.weightx = 0.3;
	    c.gridx = 0;
	    c.gridy = 1;
		jp.add(new JLabel("Loaded librarys: "),c);
		
		Box vbox = Box.createVerticalBox();
		for(String s : libs){
			JLabel lib = new JLabel(s);
			lib.setAlignmentY(Component.TOP_ALIGNMENT);
			vbox.add(lib);
		}
	    c.fill = GridBagConstraints.BOTH;
	    c.weighty = 0.75;
	    c.weightx = 0.7;
	    c.gridx = 1;
	    c.gridy = 1;
		JScrollPane jscrp = new JScrollPane(vbox);
		jp.add(jscrp,c);
		
		Box hbox = Box.createHorizontalBox();
	    c.fill = GridBagConstraints.HORIZONTAL;
	    c.weighty = 0.1;
	    c.weightx = 0.3;
	    c.gridx = 1;
	    c.gridy = 2;		
	    hbox.add(startButton);
	    hbox.add(abortButton);
		jp.add(hbox,c);
		summaryContainer.add(jp,BorderLayout.CENTER);
		return summaryContainer;
	}

	private Container drawEmptyReportContainer(){
		JPanel jp = new JPanel();
		
		JLabel emptyLabel = new JLabel("No report to show");
		jp.add(emptyLabel,BorderLayout.CENTER);
		return jp;
	}

	
	private void drawEmptyLibsContainer(){	
		jpTableList.removeAll();
		JLabel emptyLabel = new JLabel("Load a library to start...");
		jpTableList.add(emptyLabel,BorderLayout.CENTER);
		return;
	}
	
	private static void addReport(String libDatabase,File log){
		JPanel jp = new JPanel();
		jp.setLayout(new BorderLayout());
		JTabbedPane jtp = new JTabbedPane(JTabbedPane.LEFT,JTabbedPane.SCROLL_TAB_LAYOUT);	
		
		// Report		
		// Central Cut
		final JTable jte = new JTable(new JReportTableModel(libDatabase)); 
		jte.setAutoCreateRowSorter(true);
		ListSelectionModel cellSelectionModel = jte.getSelectionModel();
		final JLabel seqJLabel = new JLabel("");
		final JLabel seqFreqJLabel = new JLabel("");
		cellSelectionModel.addListSelectionListener(new ListSelectionListener() {
			@Override
			public void valueChanged(ListSelectionEvent e) { 
				String sequence=null;
				int sequenceFreq=0;

		        int[] selectedRow = jte.getSelectedRows();
	
		        for (int i = 0; i < selectedRow.length; i++) {
		        	sequence = (String) jte.getValueAt(selectedRow[i], 0);
		        	sequenceFreq = (int) jte.getValueAt(selectedRow[i], 1);		          
		        }
				seqJLabel.setText(sequence);
				seqFreqJLabel.setText(Integer.toString(sequenceFreq));
			}

		    });
		JScrollPane jscp = new JScrollPane(jte);
		jte.setAutoscrolls(true);
		
		Box seqInfo = Box.createVerticalBox();
		seqInfo.add(new JLabel("Sequence: "));
		seqInfo.add(seqJLabel);
		seqInfo.add(new JLabel("Sequence frequency: "));
		seqInfo.add(seqFreqJLabel);
		
		// Log Report
		if(log != null){
			final JTextArea logJTA = new JTextArea();
			logJTA.setLineWrap(true);
			logJTA.setWrapStyleWord(true);
			try {
				Scanner scLog = new Scanner(log);
				while(scLog.hasNext()){
					String linha = scLog.nextLine();
					if(!linha.equals(""))
						logJTA.append(scLog.nextLine()+"\n");
				}
				JScrollPane jscrlp = new JScrollPane(logJTA);
				jtp.addTab("Hunt log", logJTA);
			} catch (FileNotFoundException e1) {
				writeToLog("File "+log.getAbsolutePath()+" could not be read.");
			}
		}
		
		jtp.addTab("Central Cut",jscp);
		
		JPanel insideJp = new JPanel();
		insideJp.setLayout(new BorderLayout());
		insideJp.add(seqInfo,BorderLayout.EAST);
		insideJp.add(jtp,BorderLayout.CENTER);
		
		jp.add(insideJp,BorderLayout.CENTER);
		
		if(noReports){
			reportContainer.removeAll();
			reportContainer.setLayout(new BorderLayout());
			reportTab = new JTabbedPane(JTabbedPane.TOP,JTabbedPane.SCROLL_TAB_LAYOUT);
			reportContainer.add(reportTab);
			noReports = false;
		}
		reportTab.addTab(libDatabase,jp);
		reportTab.setSelectedIndex(reportTab.getTabCount()-1);
		return;
	}
	
	
	private JPanel drawStatusContainer(){
		JPanel statusPanel = new JPanel(new BorderLayout());

		// Cria scroll pane e adiciona statusLog dentro
		statusLog.setEditable(false);
		statusLog.append("Sequence Hunter started...");
		JScrollPane jscrlp = new JScrollPane(statusLog);	
		//jscrlp.setPreferredSize(new Dimension(250,200));
		JLabel statusLabel = new JLabel("Status: ");
				
		// Adiciona tudo na Panel
		statusPanel.add(statusLabel,BorderLayout.NORTH);
		statusPanel.add(jscrlp,BorderLayout.CENTER);
		statusPanel.add(drawProgressBarContainer(jprog),BorderLayout.SOUTH);
		return statusPanel;
	}
	
	private void fillLibContainer(){
		JTable jtabPreviewLibs;
		JScrollPane jscrlp;
		Library lib;
		Iterator<String> iterator = libs.iterator();
		
		libContainer.removeAll();
		
		while(iterator.hasNext()){
			String libPath = iterator.next();
			try{
				lib = new Library(new File(libPath));
				if(!lib.canRead()){
					throw new FileNotFoundException();
				}
				JPanel jp = new JPanel();
				final JLazyTableModel jltm = new JLazyTableModel(lib);
				jtabPreviewLibs = new JTable(jltm);
			    //SelectionListener listener = new SelectionListener(jtabPreviewLibs);
			    jtabPreviewLibs.getSelectionModel().setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
				DefaultTableCellRenderer indexRenderer = new DefaultTableCellRenderer();
				indexRenderer.setHorizontalAlignment(DefaultTableCellRenderer.CENTER);
				jtabPreviewLibs.getColumnModel().getColumn(0).setCellRenderer(new JTableRenderer(indexRenderer));
				DefaultTableCellRenderer seqRenderer = new DefaultTableCellRenderer();
				jtabPreviewLibs.getColumnModel().getColumn(1).setCellRenderer(new JTableRenderer(seqRenderer));
				jtabPreviewLibs.setAutoResizeMode(JTable.AUTO_RESIZE_OFF);
				jtabPreviewLibs.getColumnModel().getColumn(0).setPreferredWidth(40);
				jtabPreviewLibs.getColumnModel().getColumn(1).setPreferredWidth((int)(ySize*0.84));
				jtabPreviewLibs.setAutoscrolls(true);
				
				// Insere JTable dentro de JScrollPane
				jscrlp  = new JScrollPane(jtabPreviewLibs);
				jscrlp.setPreferredSize(new Dimension(900,150));
				JScrollBar jsb = jscrlp.getVerticalScrollBar();
				jsb.addAdjustmentListener(new AdjustmentListener(){
					@Override
					public void adjustmentValueChanged(AdjustmentEvent e) {
						JScrollBar jsb = (JScrollBar) e.getSource();
						int jsbMax = jsb.getMaximum();
						int jsbPos = jsb.getValue();
						if(jsbMax*0.8 <= jsbPos){
							jltm.loadMore();
						}
					}					
				});
				
				// Insere o JScrollPane dentro do JPane
				//jp.add(new JLabel(Long.toString(lib.getFileSize())));
				jp.add(jscrlp);
				
				// Adiciona aba com a lib carregada
				libContainer.addTab(lib.getFilename(),jp);
				

				writeToLog("File "+libPath+" has loaded.");
			}catch(FileNotFoundException e){
				writeToLog("File "+libPath+" could not be loaded.");
			}
			
		}
		return;
	}
		
	private Container drawProgressBarContainer(JProgressBar jprog){
		Box vbox = Box.createVerticalBox();
		
		jprog = new JProgressBar();
		jprog.setMaximum(4);
		jprog.setValue(1); // Apenas para testar
		jprog.setMinimumSize(new Dimension(ySize,xSize));
		//jprog.setVisible(false);
		
		Box hbox = Box.createHorizontalBox();
		hbox.add(processedSeqs);
		hbox.add(sensosFounded);
		hbox.add(antisensosFounded);
		hbox.add(calcSPS);
		hbox.setAlignmentX(Component.CENTER_ALIGNMENT);
		hbox.setAlignmentY(Component.CENTER_ALIGNMENT);
		vbox.add(hbox);
		vbox.add(jprog);
		
		return vbox;
	}
	
	public static void setProcessedSeqs(int n){
		Calendar cal = Calendar.getInstance();
		if(startSPS == -1){
			startSPS = cal.getTimeInMillis();
			diffSPS = -1;
		}else{
			diffSPS = (cal.getTimeInMillis() - startSPS)/1000;
			startSPS = cal.getTimeInMillis();
		}
		processedSeqs.setText("Total: "+Integer.toString(n)+" ");
	}
	public static void setSensosFounded(int n){
		sensosFounded.setText("S: "+Integer.toString(n)+" ");
	}
	public static void setAntisensosFounded(int n){
		antisensosFounded.setText("AS: "+Integer.toString(n)+" ");
	}
	public static void setSPS(){
		calcSPS.setText(" - "+diffSPS +" Sps");
	}

	public static int getProcessedSeqs(){
		return Integer.parseInt(processedSeqs.getText());
	}
	public static int getSensosFounded(){
		return Integer.parseInt(sensosFounded.getText());
	}
	public static int getAntisensosFounded(){
		return Integer.parseInt(antisensosFounded.getText());
	}
	
	public static String getTargetSeq(){
		return seqBusca.getText();
	}

	@Override
	public void actionPerformed(ActionEvent ae) {
		switch(ae.getActionCommand()){
		case "Set":
			if(seqOriginal.getSelectedText() != null)
				searchSeq = seqOriginal.getSelectedText();
			else	
				searchSeq = seqOriginal.getText();
			seqBusca.setText(searchSeq);
			writeToLog("Target sequence: " + searchSeq);
			fillLibContainer();
			summaryContainer.removeAll();
			drawSummaryContainer();
			break;
		case "Load":
			JFileChooser jfc = new JFileChooser();
			jfc.setFileFilter(new JTxtFileFilter());
			jfc.setMultiSelectionEnabled(true);
			if(jfc.showOpenDialog(null) == JFileChooser.APPROVE_OPTION){
				for(File f: jfc.getSelectedFiles()){
					String txt = f.getAbsolutePath();
					if(f.canRead()){						
						libs.add(txt);
						listModel.addElement(txt);
						writeToLog("File "+txt+" is being loaded.");
					}else{
						writeToLog("File "+txt+" can not be read.");
					}
				}
			}
			fillLibContainer();
			summaryContainer.removeAll();
			drawSummaryContainer();
			break;
		case "Unload":
			List<String> elements =jl.getSelectedValuesList();
			for(String ele: elements){
				libs.remove(ele);
				listModel.removeElement(ele);
				writeToLog("File "+ele+" unloaded.");
			}
			fillLibContainer();
			summaryContainer.removeAll();
			drawSummaryContainer();
			break;
		case "Start":
			// Monta reportContainer
			writeToLog("Starting the hunt...");
			startButton.setEnabled(false);
			abortButton.setEnabled(true);
			h = new Hunter(searchSeq,libs);
			h.start();				
			break;
		case "Abort":
			huntAbort();
			break;
		}
	}
	
	static public void writeToLog(String txt){
		statusLog.append("\n"+txt);
		statusLog.setCaretPosition(statusLog.getDocument().getLength());
		return;
	}
	
	static public void huntDone(String libDatabse,File logFile){
		if(libDatabse != null){
			addReport(libDatabse,logFile);
			jtp.setSelectedIndex(2);
			Drawer.writeToLog("Hunt done.");
			Drawer.writeToLog("Check Report tab for results...");
		}
		startButton.setEnabled(true);
		abortButton.setEnabled(false);
		startSPS = -1;
		diffSPS = 0;
	}
	
	static public void huntAbort(){
		h.stop();
		writeToLog("Hunt aborted");
		startButton.setEnabled(true);
		abortButton.setEnabled(false);
		startSPS = -1;
		diffSPS = 0;
	}

}
