package gui.toolbar;

import java.awt.Toolkit;

import javax.swing.ImageIcon;
import javax.swing.JButton;

public class SaveJButton extends JButton{
	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	private ImageIcon icon;
	private String hint;
	public SaveJButton(){
		super();
		String iconFilepath = new String("/toolbarButtonGraphics/general/Save24.gif");
		icon = new ImageIcon(Toolkit.getDefaultToolkit().getImage(getClass().getResource(iconFilepath)));
		super.setIcon(icon);
		hint = "Save a report";
		super.setToolTipText(hint);
	}
}
