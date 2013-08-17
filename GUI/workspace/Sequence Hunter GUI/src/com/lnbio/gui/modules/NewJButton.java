package com.lnbio.gui.modules;

import java.awt.Toolkit;

import javax.swing.ImageIcon;
import javax.swing.JButton;

import com.lnbio.xml.TranslationsManager;


public class NewJButton extends JButton{
	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	private ImageIcon icon;
	private String hint;
	public NewJButton(){
		super();
		String iconFilepath = new String("/toolbarButtonGraphics/general/New24.gif");
		icon = new ImageIcon(Toolkit.getDefaultToolkit().getImage(getClass().getResource(iconFilepath)));
		super.setIcon(icon);
		hint = TranslationsManager.getInstance().getText("toolbarClearHint");
		super.setToolTipText(hint);
		super.setActionCommand("Clean");
	}
}
