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
import javax.swing.table.DefaultTableModel;

import Auxiliares.JBaseTextField;
import Auxiliares.JLazyTableModel;
import Auxiliares.JReportTableModel;
import Auxiliares.JTxtFileFilter;
import Auxiliares.Library;

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
		Box vbox = Box.createVerticalBox();
		Box hbox;
		
		hbox = Box.createHorizontalBox();
		hbox.add(new Label("Target sequence: "));
		hbox.add(new Label(searchSeq));
		vbox.add(hbox);
		
		vbox.add(new Label());
		vbox.add(new Label("Loaded librarys: "));
		vbox.setMaximumSize(new Dimension(xSize,40));
		for(String s : libs){
			vbox.add(new Label(" "+s));
		}
		
		// Start cancel buttons
		JButton start = new JButton("Start");
		JButton stop = new JButton("Cancel");
		start.addActionListener(this);
		stop.addActionListener(this);
		
		hbox = Box.createHorizontalBox();
		hbox.add(start);
		hbox.add(stop);

		JPanel jp = new JPanel(new BorderLayout());
		jp.add(vbox,BorderLayout.CENTER);
		jp.add(hbox,BorderLayout.SOUTH);
		
		JScrollPane jscrp = new JScrollPane(jp);
		
		summaryContainer.add(jscrp,BorderLayout.CENTER);
		return summaryContainer;
	}
	
	private Container drawReportContainer(File f){
		JPanel jp = new JPanel();
		jp.setLayout(new BorderLayout());
		JTabbedPane jtp= new JTabbedPane(JTabbedPane.LEFT,JTabbedPane.SCROLL_TAB_LAYOUT);	
		
		// Report 1		
		JTable jte = new JTable(new JReportTableModel(f)); 
		jtp.addTab("Central Cut",jte);
		Box seqInfo = Box.createVerticalBox();
		seqInfo.add(new Label("Sequence:"));
		seqInfo.add(new Label("Sequence frequency:"));
		
		JPanel insideJp = new JPanel();
		insideJp.setLayout(new BorderLayout());
		insideJp.add(seqInfo,BorderLayout.EAST);
		insideJp.add(jtp,BorderLayout.CENTER);
		
		jp.add(insideJp,BorderLayout.CENTER);
		return jp;
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
			jtp.addTab("Report",null,drawReportContainer(new File("resultados")),"Check the results after a hunt");
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
