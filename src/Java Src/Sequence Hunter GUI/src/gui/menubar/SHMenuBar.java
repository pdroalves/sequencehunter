package gui.menubar;

import gui.Drawer;
import gui.ReportDrawer;

import java.awt.Color;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.BorderFactory;
import javax.swing.JDialog;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JTabbedPane;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import xml.TranslationsManager;

import dialogs.AboutDialog;

public class SHMenuBar implements ChangeListener, ActionListener{

	private TranslationsManager tm;
	public final int MENUBAR_NO_REPORT_MODE = 0;
	public final int MENUBAR_REPORT_MODE = 1;
	//private int currentMode;
	private JMenu menuFile;
	private JMenu menuHelp;
	private JMenu menuItemExport;
	private JMenuItem menuItemExportSingleReport;
	private JMenuItem menuItemExportEntireReport;

	public SHMenuBar(){
		this.tm = TranslationsManager.getInstance();

		// Export set
		menuItemExport = new JMenu(tm.getText("menuFileItemExportGeneric"));
		menuItemExportSingleReport = new JMenuItem(tm.getText("menuFileItemExportGenericSingleReport"));
		menuItemExportSingleReport.setActionCommand("ExportSingle");
		menuItemExportEntireReport = new JMenuItem(tm.getText("menuFileItemExportGenericEntireReport"));
		menuItemExportEntireReport.setActionCommand("ExportAll");
		
		menuItemExport.setBorder(BorderFactory.createLineBorder(Color.BLACK));
		menuItemExport.setBorderPainted(true);

		menuItemExportSingleReport.addActionListener(this);
		menuItemExportEntireReport.addActionListener(this);
		
		menuItemExport.add(menuItemExportSingleReport);
		menuItemExport.add(menuItemExportEntireReport);
	}

	@Override
	public void stateChanged(ChangeEvent e) {
		if(e.getSource() instanceof JTabbedPane){
			JTabbedPane jtp = (JTabbedPane) e.getSource();
			int mode;
			
			if(jtp.getSelectedIndex() == Drawer.REPORT_TAB &&
					ReportDrawer.getReportsLoaded() > 0){
				mode = MENUBAR_REPORT_MODE;
			}else{
				mode = MENUBAR_NO_REPORT_MODE;				
			}

			if(	menuFile != null && 
					menuHelp != null){

				if(mode == MENUBAR_NO_REPORT_MODE){
					menuFile.remove(menuItemExport);
				}else if(mode == MENUBAR_REPORT_MODE){
					menuFile.add(menuItemExport,0);
				}else{
					return;
				}

				//currentMode = mode;
			}
		}
	}

	public JMenuBar getJMenuBar(){
		// Barra do menu
		JMenuBar menuBar = new JMenuBar();

		// Novo Menu  
		menuFile = new JMenu(tm.getText("menuFileLabel")); 
		menuHelp = new JMenu(tm.getText("menuHelpLabel"));   

		// Item do menu  
		final JMenuItem menuItemExit = new JMenuItem(tm.getText("menuFileItemExitLabel"));  		
		JMenuItem menuItemAbout = new JMenuItem(tm.getText("menuHelpItemAboutLabel"));

		menuItemExit.setBorder(BorderFactory.createLineBorder(Color.BLACK));
		menuItemExit.setBorderPainted(true);
		menuItemAbout.setBorder(BorderFactory.createLineBorder(Color.BLACK));
		menuItemAbout.setBorderPainted(true);

		menuItemExit.addActionListener(new ActionListener(){

			@Override
			public void actionPerformed(ActionEvent e) {
				Drawer.dispose();
			}


		});

		menuItemAbout.addActionListener(new ActionListener() {

			@Override
			public void actionPerformed(ActionEvent arg0) {
				JDialog about = new AboutDialog(Drawer.getJFrame());
				about.setVisible(true);
			}
		});

		menuFile.addSeparator();
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

	@Override
	public void actionPerformed(ActionEvent e) {
		if(e.getActionCommand().equals("ExportSingle")){
			System.err.println("Aba selecionada: "+ReportDrawer.getSelectedReportTabIndex()+"->"+ReportDrawer.getSelectedSubReportTabIndex());
		}else if(e.getActionCommand().equals("ExportAll")){			
			System.err.println("Aba selecionada: "+ReportDrawer.getSelectedReportTabIndex());
		}		
	}
}
