package com.lnbio.gui.modules;


import java.awt.Color;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.Observable;
import java.util.Observer;
import javax.swing.BorderFactory;
import javax.swing.ImageIcon;
import javax.swing.JDialog;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;

import com.lnbio.dialogs.AboutDialog;
import com.lnbio.dialogs.ExportDialog;
import com.lnbio.dialogs.SettingsDialog;
import com.lnbio.gui.Drawer;
import com.lnbio.gui.ReportDrawer;
import com.lnbio.gui.SearchDrawer;
import com.lnbio.hunt.Hunter;
import com.lnbio.xml.TranslationsManager;



public class SHMenuBar implements Observer, ActionListener{

	private TranslationsManager tm;
	public final int MENUBAR_NO_REPORT_MODE = 0;
	public final int MENUBAR_REPORT_MODE = 1;
	//private int currentMode;
	private JMenu menuFile;
	private JMenu menuHelp;
	private JMenuItem menuItemExport;
	private Hunter h;

	public SHMenuBar(Hunter h){
		this.tm = TranslationsManager.getInstance();
		menuItemExport = new JMenuItem(tm.getText("menuFileItemExportGeneric"));
		this.h = h;
	}

	public JMenuBar getJMenuBar(){
		// Barra do menu
		JMenuBar menuBar = new JMenuBar();

		// Novo Menu  
		menuFile = new JMenu(tm.getText("menuFileLabel")); 
		menuHelp = new JMenu(tm.getText("menuHelpLabel"));   		
		
		// Item do menu  
		JMenuItem menuItemNew = new JMenuItem(tm.getText("menuFileItemNew"));
		JMenuItem menuItemOpenLibrary = new JMenuItem(tm.getText("menuFileItemOpenLibrary"));
		JMenuItem menuItemExit = new JMenuItem(tm.getText("menuFileItemExitLabel"));  		
		JMenuItem menuItemAbout = new JMenuItem(tm.getText("menuHelpItemAboutLabel"));
		menuItemExport.setEnabled(false);
		JMenuItem menuItemSettings = new JMenuItem(tm.getText("menuFileItemSettingsLabel"));
		
		// ActionCommand
		menuItemNew.setActionCommand("New");
		menuItemOpenLibrary.setActionCommand("Open");
		menuItemExit.setActionCommand("Exit");
		menuItemAbout.setActionCommand("About");
		menuItemExport.setActionCommand("Export");
		menuItemSettings.setActionCommand("Settings");
		
		// Icons
		ImageIcon iconNew = new ImageIcon(Toolkit.getDefaultToolkit().getImage(getClass().getResource("/toolbarButtonGraphics/general/New16.gif")));
		ImageIcon iconOpen = new ImageIcon(Toolkit.getDefaultToolkit().getImage(getClass().getResource("/toolbarButtonGraphics/general/Open16.gif")));
		ImageIcon iconExit = new ImageIcon(Toolkit.getDefaultToolkit().getImage(getClass().getResource("/toolbarButtonGraphics/general/Stop16.gif")));
		ImageIcon iconAbout = new ImageIcon(Toolkit.getDefaultToolkit().getImage(getClass().getResource("/toolbarButtonGraphics/general/Information16.gif")));
		ImageIcon iconExport = new ImageIcon(Toolkit.getDefaultToolkit().getImage(getClass().getResource("/toolbarButtonGraphics/general/Export16.gif")));
		ImageIcon iconSettings = new ImageIcon(Toolkit.getDefaultToolkit().getImage(getClass().getResource("/toolbarButtonGraphics/general/Preferences16.gif")));
		menuItemNew.setIcon(iconNew);
		menuItemOpenLibrary.setIcon(iconOpen);
		menuItemExit.setIcon(iconExit);
		menuItemAbout.setIcon(iconAbout);
		menuItemExport.setIcon(iconExport);
		menuItemSettings.setIcon(iconSettings);
		//menuItemExit.setBorder(BorderFactory.createLineBorder(Color.BLACK));
		//menuItemExit.setBorderPainted(true);
		//menuItemAbout.setBorder(BorderFactory.createLineBorder(Color.BLACK));
		//menuItemAbout.setBorderPainted(true);
		//menuItemExport.setBorder(BorderFactory.createLineBorder(Color.BLACK));
		//menuItemExport.setBorderPainted(true);
		menuItemNew.addActionListener(this);
		menuItemOpenLibrary.addActionListener(this);
		menuItemExport.addActionListener(this);
		menuItemExit.addActionListener(this);
		menuItemAbout.addActionListener(this);
		menuItemSettings.addActionListener(this);
		
		menuFile.add(menuItemNew);
		menuFile.add(menuItemOpenLibrary);
		menuFile.addSeparator();
		menuFile.add(menuItemExport);
		menuFile.add(menuItemSettings);
		menuFile.addSeparator();
		menuFile.add(menuItemExit);
		
		menuHelp.add(menuItemAbout);
		
		menuBar.add(menuFile); 
		menuBar.add(menuHelp);

		menuBar.setBorder(BorderFactory.createLineBorder(Color.BLACK));
		menuBar.setBorderPainted(true);

		return menuBar;
	}

	@Override
	public void actionPerformed(ActionEvent e) {
		switch(e.getActionCommand()){
		case "New":
			Drawer.getSearchDrawer().cleanHunt();
			break;		
		case "Open":
			Drawer.getReportDrawer().openLibrary();
			break;
		case "Export":
			// Exporta uma aba especifica do relatorio aberto
			ExportDialog ed = new ExportDialog(Drawer.getJFrame());
			ed.setVisible(true);
			break;
		case "Exit":
			Drawer.dispose();
			break;
		case "About":
			JDialog about = new AboutDialog(Drawer.getJFrame());
			about.setVisible(true);
			break;
		case "Settings":
			JDialog settings = new SettingsDialog(Drawer.getJFrame(),h);
			settings.setVisible(true);
			break;
		}
	}

	@Override
	public void update(Observable o, Object arg) {
		//if(arg instanceof ReportDrawer){
			int reportsCount = ReportDrawer.getReportsLoaded();
			int mode;

			if(reportsCount > 0){
				mode = MENUBAR_REPORT_MODE;
			}else{
				mode = MENUBAR_NO_REPORT_MODE;				
			}

			if(	menuFile != null && 
					menuHelp != null){

				if(mode == MENUBAR_NO_REPORT_MODE){
					menuItemExport.setEnabled(false);
				}else if(mode == MENUBAR_REPORT_MODE){
					menuItemExport.setEnabled(true);
				}else{
					return;
				}

				//currentMode = mode;
			}
		//}

	}
}
