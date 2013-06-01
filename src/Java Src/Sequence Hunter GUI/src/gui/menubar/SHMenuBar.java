package gui.menubar;

import gui.Drawer;
import gui.ReportDrawer;
import hunt.Hunter;

import java.awt.Color;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.Observable;
import java.util.Observer;
import javax.swing.BorderFactory;
import javax.swing.JDialog;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;

import xml.TranslationsManager;

import dialogs.AboutDialog;
import dialogs.ExportDialog;
import dialogs.SettingsDialog;

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
		JMenuItem menuItemExit = new JMenuItem(tm.getText("menuFileItemExitLabel"));  		
		JMenuItem menuItemAbout = new JMenuItem(tm.getText("menuHelpItemAboutLabel"));
		menuItemExport.setEnabled(false);
		JMenuItem menuItemSettings = new JMenuItem(tm.getText("menuFileItemSettingsLabel"));
		
		menuItemExit.setActionCommand("Exit");
		menuItemAbout.setActionCommand("About");
		menuItemExport.setActionCommand("Export");
		menuItemSettings.setActionCommand("Settings");
		
		//menuItemExit.setBorder(BorderFactory.createLineBorder(Color.BLACK));
		//menuItemExit.setBorderPainted(true);
		//menuItemAbout.setBorder(BorderFactory.createLineBorder(Color.BLACK));
		//menuItemAbout.setBorderPainted(true);
		//menuItemExport.setBorder(BorderFactory.createLineBorder(Color.BLACK));
		//menuItemExport.setBorderPainted(true);

		menuItemExport.addActionListener(this);
		menuItemExit.addActionListener(this);
		menuItemAbout.addActionListener(this);
		menuItemSettings.addActionListener(this);
		
		menuFile.add(menuItemSettings);
		menuFile.add(menuItemExport);
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
		if(e.getActionCommand().equals("Export")){
			// Exporta uma aba especifica do relatorio aberto
			ExportDialog ed = new ExportDialog(Drawer.getJFrame());
			ed.setVisible(true);
		}else if(e.getActionCommand().equals("Exit")){
			Drawer.dispose();
		}else if(e.getActionCommand().equals("About")){
			JDialog about = new AboutDialog(Drawer.getJFrame());
			about.setVisible(true);
		}else if(e.getActionCommand().equals("Settings")){
			JDialog settings = new SettingsDialog(Drawer.getJFrame(),h);
			settings.setVisible(true);
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
