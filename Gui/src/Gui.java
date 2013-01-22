import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.AdjustmentEvent;
import java.awt.event.AdjustmentListener;
import java.io.File;
import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import javax.swing.*;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;
import javax.swing.table.DefaultTableModel;

import Auxiliares.JBaseTextField;
import Auxiliares.JLazyTableModel;
import Auxiliares.JReportTableModel;
import Auxiliares.JTxtFileFilter;
import Auxiliares.Library;
import Dialogs.AboutDialog;

public class Gui implements ActionListener {
	
	private JFrame jfrm;
	private JTabbedPane jtp;
	private JBaseTextField seqOriginal;
	private String searchSeq;
	private JLabel seqBusca;
	private JButton setSeqButton;
	private static JTextArea statusLog;
	private JTextField outputDir;
	private JButton startstopButton = new JButton("Start");
	private JProgressBar jprog;
	private JList<String> jl ;
	private DefaultListModel<String> listModel;
	private ArrayList<String> libs = new ArrayList<String>();
	private JTabbedPane libContainer;
	private JPanel summaryContainer;
	private Container reportContainer;
	private JTabbedPane reportTab;
	private Boolean noReports = true;
	private int xSize = 700;
	private int ySize = 1000;
	
	Gui(){
		seqOriginal = new JBaseTextField(25);
		seqBusca = new JLabel();
		statusLog = new JTextArea();
		jl = new JList<String>();
		listModel = new DefaultListModel<String>();  
		libContainer = new JTabbedPane(JTabbedPane.TOP,JTabbedPane.SCROLL_TAB_LAYOUT);	
		libContainer.setPreferredSize(new Dimension(900,300));
		summaryContainer = new JPanel(new BorderLayout());
		reportContainer = drawEmptyReportContainer();
		
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
		
		jfrm.add(jtp,BorderLayout.CENTER);
		
		// Monta statusContainer
		jfrm.add(drawStatusContainer(),BorderLayout.SOUTH);
				
		jfrm.setVisible(true);
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
		
		// Configura tab para libs
		libs.setLayout(new GridLayout(2,2));
		libs.add(new JLabel("Libraries loaded: "));
		JScrollPane jscrlp = new JScrollPane(jl);
		JButton loadLib = new JButton("Load");
		JButton unloadLib = new JButton("Unload");
		loadLib.addActionListener(this);
		unloadLib.addActionListener(this);
		jl.setModel(listModel);
		
		hbox = Box.createHorizontalBox();
		hbox.add(loadLib);
		hbox.add(unloadLib);
		libs.add(jscrlp);		
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
		
		// Start cancel buttons
		JButton start = new JButton("Start");
		JButton stop = new JButton("Cancel");
		start.addActionListener(this);
		stop.addActionListener(this);
		
		Box hbox = Box.createHorizontalBox();
	    c.fill = GridBagConstraints.HORIZONTAL;
	    c.weighty = 0.1;
	    c.weightx = 0.3;
	    c.gridx = 1;
	    c.gridy = 2;		
	    hbox.add(start);
	    hbox.add(stop);
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
	
	private void addReport(File f){
		JPanel jp = new JPanel();
		jp.setLayout(new BorderLayout());
		JTabbedPane jtp = new JTabbedPane(JTabbedPane.LEFT,JTabbedPane.SCROLL_TAB_LAYOUT);	
		
		// Report		
		// Tabela
		final JTable jte = new JTable(new JReportTableModel(f)); 
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
		
		jtp.addTab("Central Cut",jte);
		Box seqInfo = Box.createVerticalBox();
		seqInfo.add(new JLabel("Sequence: "));
		seqInfo.add(seqJLabel);
		seqInfo.add(new JLabel("Sequence frequency: "));
		seqInfo.add(seqFreqJLabel);
		
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
			reportTab.addTab(f.getName(),jp);
			reportTab.setSelectedIndex(reportTab.getTabCount()-1);
		return;
	}
	
	
	private Container drawStatusContainer(){
		Box vbox = Box.createVerticalBox();
		
		// Cria scroll pane e adiciona statusLog dentro
		statusLog.setEditable(false);
		statusLog.append("Sequence Hunter started...");
		JScrollPane jscrlp = new JScrollPane(statusLog);	
		jscrlp.setPreferredSize(new Dimension(250,200));
		
		// Adiciona tudo na vbox
		JLabel statusLabel = new JLabel("Status: ");
		vbox.add(statusLabel);
		vbox.add(jscrlp);

		// Monta progressBarContainer
		vbox.add(drawProgressBarContainer(jprog));
		return vbox;
	}
	
	private void fillLibContainer(){
		JTable jtabPreviewLibs;
		String[] headings = { "Sequence Preview" };
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
		jprog.setValue(3); // Apenas para testar
		jprog.setMinimumSize(new Dimension(ySize,xSize));
		//jprog.setVisible(false);
		vbox.add(jprog);
		
		
		return vbox;
	}

	@Override
	public void actionPerformed(ActionEvent ae) {
		if(ae.getActionCommand().equals("Set")){
			searchSeq = new String(seqOriginal.getSelectedText());
			seqBusca.setText(searchSeq);
			writeToLog("Target sequence: " + searchSeq);
		}
		if(ae.getActionCommand().equals("Load")){
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
				fillLibContainer();
			}
		}
		if(ae.getActionCommand().equals("Unload")){
			List<String> elements =jl.getSelectedValuesList();
			for(String ele: elements){
				libs.remove(ele);
				listModel.removeElement(ele);
				writeToLog("File "+ele+" unloaded.");
			}
			fillLibContainer();
		}
		if(ae.getActionCommand().equals("Start")){
			// Monta reportContainer
			writeToLog("Starting the hunt...");
			/////////////////////////////////////////
			/////// Chamada para shunter-cmd////////
			////////////////////////////////////////
			writeToLog("Hunt done.");
			writeToLog("Check Report tab for results...");
			File libFile = new File("resultados");
			addReport(libFile);
			jtp.setSelectedIndex(2);
		}else{
			summaryContainer.removeAll();
			drawSummaryContainer();
		}
	}
	
	static public void writeToLog(String txt){
		statusLog.append("\n"+txt);
		return;
	}

}
