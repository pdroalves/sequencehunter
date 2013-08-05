package com.lnbio.auxiliares;

import java.awt.Color;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.BorderFactory;
import javax.swing.JMenuItem;
import javax.swing.JPopupMenu;

import com.lnbio.database.DBManager;
import com.lnbio.gui.ReportDrawer;
import com.lnbio.xml.TranslationsManager;

@SuppressWarnings("serial")
public class FiveCutPopUpMenu extends JPopupMenu implements ActionListener {
    private int tableIndex;
    private String centralCutSequence;
    private ReportDrawer owner; 
    private DBManager dbm;
    
	public FiveCutPopUpMenu(ReportDrawer owner,DBManager dbm,String centralCutSequence,int tableIndex){
		this.tableIndex = tableIndex;
		this.centralCutSequence = centralCutSequence;
		this.owner = owner;
		this.dbm = dbm;
		
    	JMenuItem anItem = new JMenuItem(TranslationsManager.getInstance().getText("genFiveCutReportFromCentralCutSeq"));
    	anItem.addActionListener(this);
    	
        this.add(anItem);
        this.setBorder(BorderFactory.createLineBorder(Color.gray));
    }

	@Override
	public void actionPerformed(ActionEvent arg0) {
		System.out.println("Calculando fivecut da sequencia "+centralCutSequence);
		try {
			owner.addFiveCutSubReport(dbm,tableIndex, centralCutSequence);
		} catch (Exception e) {
			System.err.println(e.getMessage());
		}
	}
}