package com.lnbio.gui.modules;

import java.awt.Toolkit;

import javax.swing.ImageIcon;
import javax.swing.JButton;

import com.lnbio.xml.TranslationsManager;


public class OpenJButton extends JButton{
	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	private ImageIcon icon;
	private String hint;
	public OpenJButton(){
		super();
		String iconFilepath = new String("/toolbarButtonGraphics/general/Open24.gif");
		icon = new ImageIcon(Toolkit.getDefaultToolkit().getImage(getClass().getResource(iconFilepath)));
		super.setIcon(icon);
		hint = TranslationsManager.getInstance().getText("toolbarOpenHint");
		super.setToolTipText(hint);
		super.setActionCommand("Open");
	}
}
