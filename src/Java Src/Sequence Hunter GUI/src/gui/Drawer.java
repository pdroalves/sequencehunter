package gui;
import gui.toolbar.NewJButton;
import gui.toolbar.OpenJButton;
import gui.toolbar.SaveAllJButton;
import gui.toolbar.SaveJButton;
import hunt.Hunter;

import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;
import java.util.ArrayList;
import java.util.Iterator;

import javax.swing.*;
import javax.swing.border.BevelBorder;
import javax.swing.border.EtchedBorder;

import dialogs.AboutDialog;


public class Drawer {

	private JFrame jfrm;
	private static JTabbedPane jtp;
	private static JTextArea statusLog;
	private static Container jcprogress;
	private int xSize = 600;
	private int ySize = 800;
	private SearchDrawer searchDrawer;
	private SummaryDrawer summaryDrawer;
	private ReportDrawer reportDrawer; 
	private static JLabel processedSeqs;
	private static JLabel sensosFounded;
	private static JLabel antisensosFounded;
	private static JLabel calcSPS;
	private static JProgressBar jprog;
	private static Hunter h;
	private static JToolBar toolbar;
	private static JMenuBar menubar;

	public Drawer(){
		statusLog = new JTextArea();
		statusLog.setLineWrap(true);
		statusLog.setWrapStyleWord(true); 
		processedSeqs = new JLabel("");
		sensosFounded = new JLabel("");
		antisensosFounded = new JLabel("");
		calcSPS = new JLabel("");
		h = new Hunter();
		jprog = new JProgressBar();
		jcprogress = drawProgressBarContainer(jprog);

		// Cria JFrame container
		jfrm = new JFrame("Sequence Hunter");
		jfrm.setResizable(true);
	
		// Set look and feel
        try {
        	if(Hunter.getOS().equals("WIN"))
    			UIManager.setLookAndFeel("javax.swing.plaf.windows.WindowsLookAndFeel");
        	else
        		UIManager.setLookAndFeel("com.sun.java.swing.plaf.gtk.GTKLookAndFeel");
		} catch (ClassNotFoundException | InstantiationException
				| IllegalAccessException | UnsupportedLookAndFeelException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

		// Seta FlowLayout para o content pane
		jfrm.getContentPane().setLayout(new BorderLayout());

		jfrm.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		jfrm.setSize(ySize,xSize);
		jfrm.setLocationByPlatform(true);
		jfrm.setLocationRelativeTo(null);

		//Gera menu
		menubar = drawMenuBar();
		jfrm.setJMenuBar(menubar);

		//Seta posicao inicial para centro da tela
		//jfrm.setLocationRelativeTo(null);	
		
		// Cria tabbed pane
		jtp = new JTabbedPane(JTabbedPane.TOP,JTabbedPane.SCROLL_TAB_LAYOUT);	
		searchDrawer = new SearchDrawer(xSize, ySize, h);
		summaryDrawer = new SummaryDrawer(this, h);
		reportDrawer = new ReportDrawer();

		// Monta searchContainer		
		jtp.addTab("Setup",null,searchDrawer.getContainer(),"Set what you want to search");

		// Monta summaryContainer
		jtp.addTab("Summary",null,summaryDrawer.getContainer(),"Confirm the configuration and start the hunt");

		// Monta reportContainer
		jtp.addTab("Report",null,reportDrawer.getContainer(),"Check the results after a hunt");

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
		
		jsp.setDividerLocation(0.70);
		jsp.setResizeWeight(0.5);
		jsp.setMaximumSize(new Dimension(xSize/3,ySize));
		jsp.setBorder(BorderFactory.createBevelBorder(BevelBorder.RAISED));
		
		toolbar = drawToolbar();
		toolbar.setBorder(BorderFactory.createEtchedBorder(EtchedBorder.RAISED));
		jfrm.add(toolbar,BorderLayout.NORTH);
		jfrm.add(jsp,BorderLayout.CENTER);
		jfrm.setVisible(true);

	}

	private JToolBar drawToolbar(){
		JToolBar jtb = new JToolBar();
		// Botoes
		NewJButton newhunt = new NewJButton();
		OpenJButton open = new OpenJButton();
		JButton save = new SaveJButton();
		JButton saveall = new SaveAllJButton();
		
		// Actions
		newhunt.addActionListener(searchDrawer);
		open.addActionListener(reportDrawer);
		
		jtb.add(newhunt);
		jtb.add(open);
		jtb.add(save);
		jtb.add(saveall);
		jtb.setFloatable(false);
		return jtb;
	}
	
	private JMenuBar drawMenuBar(){
		// Barra do menu
		JMenuBar menuBar = new JMenuBar();

		// Novo Menu  
		JMenu menuFile = new JMenu("File"); 
		JMenu menuHelp = new JMenu("Help");   

		// Item do menu  
		final JMenuItem menuItemExit = new JMenuItem("Exit");  		
		JMenuItem menuItemAbout = new JMenuItem("About");

		menuItemExit.setBorder(BorderFactory.createLineBorder(Color.BLACK));
		menuItemExit.setBorderPainted(true);
		menuItemAbout.setBorder(BorderFactory.createLineBorder(Color.BLACK));
		menuItemAbout.setBorderPainted(true);

		menuItemExit.addActionListener(new ActionListener(){

			@Override
			public void actionPerformed(ActionEvent e) {
				jfrm.dispose();
			}
			
			
		});
		
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
		
		menuFile.setBorder(BorderFactory.createLoweredBevelBorder());
		menuHelp.setBorder(BorderFactory.createLoweredBevelBorder());
		menuBar.setBorder(BorderFactory.createLoweredBevelBorder());
		menuBar.setBorderPainted(true);

		return menuBar;
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
		statusPanel.add(jcprogress,BorderLayout.SOUTH);
		return statusPanel;
	}

	protected void initProgressBar(int max){
		jprog.setMaximum(max);
		jprog.setValue(0);
		jprog.setVisible(true);
	}
	
	protected int getTotalLibSize(ArrayList<String> libs){
		int count = 0;
		Iterator<String> iterator = libs.iterator();
		while(iterator.hasNext()){
			File f = new File(iterator.next());
			count += f.length();
		}
		System.out.println("Lib size: "+count);
		return count;
	}

	static public void writeToLog(String txt){
		statusLog.append("\n"+txt);
		statusLog.setCaretPosition(statusLog.getDocument().getLength());
		return;
	}
	
	private Container drawProgressBarContainer(JProgressBar jprog){
		Box vbox = Box.createVerticalBox();
		
		jprog.setMinimumSize(new Dimension(ySize,xSize));
		jprog.setVisible(false);
		
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
	
	
	
	public static void updateProgressBar(int br){
		jprog.setValue(br);
	}
	
	public static void setProcessedSeqs(int n){
		processedSeqs.setText("Total: "+Integer.toString(n)+" ");
	}
	public static void setSensosFounded(int n){
		sensosFounded.setText("S: "+Integer.toString(n)+" ");
	}
	public static void setAntisensosFounded(int n){
		antisensosFounded.setText("AS: "+Integer.toString(n)+" ");
	}
	public static void setSPS(int n){
		calcSPS.setText(" - "+Integer.toString(n) +" Events/s");
	}
	
	public static void enableStatusJLabels(boolean b){
		processedSeqs.setVisible(b);
		sensosFounded.setVisible(b);
		antisensosFounded.setVisible(b);
		calcSPS.setVisible(b);
		jprog.setVisible(b);
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
	
	protected static void moveToReportTab(){
		jtp.setSelectedIndex(2);		
	}

}
