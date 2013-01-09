import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.ArrayList;

import javax.swing.*;
import javax.swing.table.DefaultTableModel;

import Auxiliares.JBaseTextField;

public class Gui implements ActionListener {
	
	JFrame jfrm;
	JBaseTextField seqOriginal;
	String searchSeq;
	JLabel seqBusca;
	JButton setSeqButton;
	JTextArea statusLog;
	JTextField outputDir;
	JButton startstopButton = new JButton("Start");
	JProgressBar jprog;
	int xSize = 700;
	int ySize = 1000;
	
	Gui(){
		seqOriginal = new JBaseTextField(25);
		seqBusca = new JLabel();
		statusLog = new JTextArea();
		
		// Cria JFrame container
		jfrm = new JFrame("Sequence Hunter");
		jfrm.setResizable(false);
		
		//Gera menu
		jfrm.setJMenuBar(drawMenuBar());
		
		//Seta posicao inicial para centro da tela
		//jfrm.setLocationRelativeTo(null);
		
		// Seta FlowLayout para o content pane
		jfrm.getContentPane().setLayout(new BorderLayout());
				
		jfrm.setSize(ySize,xSize);
		jfrm.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		
		// Cria tabbed pane
		JTabbedPane jtp = new JTabbedPane(JTabbedPane.TOP,JTabbedPane.SCROLL_TAB_LAYOUT);	
		
		// Monta searchContainer
		jtp.addTab("Setup",drawSearchContainer());
		
		// Monta summaryContainer
		jtp.addTab("Summary",Box.createVerticalBox());
		
		// Monta reportContainer
		jtp.addTab("Report",Box.createVerticalBox());
		
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
		libs.setLayout(new GridLayout(1,1));
		libs.add(new JLabel("To-do"));
		
		
		// Monta libContainer
		ArrayList<Object[][]> tmp = new ArrayList<Object[][]>();
		String[][] hold1 = {{
				"Seq0"},
				{"Seq1"},
					{"Seq2"}};
		String[][] hold2 = {{
				"Seq3"},
				{"Seq4"},
					{"Seq5"}};
		String[][] hold3 = {{
				"Seq6"},
				{"Seq7"},
					{"Seq8"}};
		tmp.add(hold1);
		tmp.add(hold2);
		tmp.add(hold3);
		

		// Adiciona tabs
		Box vbox = Box.createVerticalBox();
		vbox.add(seqBuscaPanel);
		vbox.add(libs);
		vbox.add(drawLibContainer(tmp,3));
		
		
		return vbox;
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
	
	private Container drawLibContainer(ArrayList<Object[][]> data,int nLibs){
		// Cria tabbed pane
		JTabbedPane jtp = new JTabbedPane(JTabbedPane.TOP,JTabbedPane.SCROLL_TAB_LAYOUT);	

		JTable jtabPreviewLibs;
		String[] headings = { "Sequence Preview" };
		JScrollPane jscrlp;
		
		for(int i = 0; i < nLibs; i++){
			JPanel jp = new JPanel();
			jtabPreviewLibs = new JTable(new DefaultTableModel(data.get(i),headings));
			jscrlp  = new JScrollPane(jtabPreviewLibs);
			jscrlp.setPreferredSize(new Dimension(900,200));
			jp.add(jscrlp);
			jtp.addTab(new String("Lib "+i),jp);
		}
		
		return jtp;
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
			statusLog.append("\n"+"Target sequence: " + searchSeq);
		}
	}
	

}
