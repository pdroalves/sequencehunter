package com.lnbio.auxiliares;


import javax.swing.*;

import com.lnbio.gui.ReportDrawer;

import java.awt.*;
import java.awt.event.*;

/**
 * Component to be used as tabComponent;
 * Contains a JLabel to show the text and 
 * a JButton to close the tab it belongs to 
 */ 
public class RemovableTabComponent extends JPanel implements ActionListener{
    /**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	private JTabbedPane pane;
	private JButton closeButton;
	private ImageIcon icon;
	private ReportDrawer rd;
	private int tabIndex;
	
    public RemovableTabComponent(final ReportDrawer rd,final JTabbedPane pane,int tabIndex) {
    	this.pane = pane;
    	this.rd = rd;
    	this.tabIndex = tabIndex; 
    	
    	// Cria close button
    	closeButton = new JButton();
    	closeButton.setToolTipText("Close");
    	closeButton.setActionCommand("Close Tab");
    	closeButton.addActionListener(this);
    	
    	// Adiciona imagem no botao
    	String iconFilepath = new String("/toolbarButtonGraphics/general/Delete16.gif");
    	icon = new ImageIcon(Toolkit.getDefaultToolkit().getImage(getClass().getResource(iconFilepath)));
    	closeButton.setIcon(icon);
    	closeButton.setPreferredSize(new Dimension(20,20));
    	closeButton.setOpaque(false);
    	closeButton.setBorder(BorderFactory.createEmptyBorder());
    	closeButton.setBorderPainted(false);
    	
    	this.add(new JLabel(pane.getTitleAt(tabIndex)));
    	this.add(closeButton);
    	this.setOpaque(false);
    }
    
    @Override
	public void actionPerformed(ActionEvent e) {
    		rd.removeReport(tabIndex);
		    rd.updateReportsView();
	}
    
 }


