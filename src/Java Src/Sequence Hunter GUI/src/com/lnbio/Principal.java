package com.lnbio;

import javax.swing.SwingUtilities;

import com.lnbio.gui.Drawer;

public class Principal {

	public static void main(String[] args) {
		SwingUtilities.invokeLater(new Runnable() {
			public void run(){
				new Drawer();
			}
		});
	}
}
