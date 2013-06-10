package com.lnbio.dialogs;


import java.awt.BorderLayout;
import java.awt.Component;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;

import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.ButtonGroup;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JDialog;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.border.EmptyBorder;

import com.lnbio.gui.Drawer;
import com.lnbio.hunt.Hunter;
import com.lnbio.xml.TranslationsManager;


@SuppressWarnings("serial")
public class SettingsDialog extends JDialog implements ActionListener {

	private TranslationsManager tm = TranslationsManager.getInstance();
	private int mode;
	private Hunter h;

	public SettingsDialog(JFrame parent,Hunter h){
		super(parent, "Settings", true);
		this.h = h;
		mode = h.getMode();
		JPanel panel = new JPanel(new BorderLayout());
		panel.setBorder(new EmptyBorder(10,10,10,10));

		Box vertical = new Box(BoxLayout.Y_AXIS);

		vertical.setAlignmentX(LEFT_ALIGNMENT);
		JLabel forceCudaModeLabel = new JLabel(tm.getText("ForceCudaMode"));
		JLabel forceNonCudaModeLabel = new JLabel(tm.getText("ForceNonCudaMode"));
		JLabel nonForceModeLabel = new JLabel(tm.getText("NonForceMode"));
		final JCheckBox forceCudaModeCheckBox = new JCheckBox();
		final JCheckBox forceNonCudaModeCheckBox = new JCheckBox();
		JCheckBox nonForceModeCheckBox = new JCheckBox();
		JButton okButton = new JButton(tm.getText("SettingsDialogOkButton"));
		JButton cancelButton = new JButton(tm.getText("SettingsDialogCancelButton"));

		forceCudaModeCheckBox.setActionCommand("CudaMode");
		forceCudaModeCheckBox.addItemListener(new ItemListener(){

			@Override
			public void itemStateChanged(ItemEvent e) {
				if(e.getStateChange() == ItemEvent.SELECTED){
					mode = Hunter.FORCE_CUDA_MODE;
				}
			}

		});
		forceCudaModeCheckBox.setActionCommand("NonCudaMode");
		forceNonCudaModeCheckBox.addItemListener(new ItemListener(){

			@Override
			public void itemStateChanged(ItemEvent e) {
				if(e.getStateChange() == ItemEvent.SELECTED){
					mode = Hunter.FORCE_NONCUDA_MODE;
				}				
			}

		});
		forceCudaModeCheckBox.setActionCommand("NonForceMode");
		nonForceModeCheckBox.addItemListener(new ItemListener(){

			@Override
			public void itemStateChanged(ItemEvent e) {
				if(e.getStateChange() == ItemEvent.SELECTED){
					mode = Hunter.NON_FORCE_MODE;
					forceCudaModeCheckBox.setEnabled(false);
					forceNonCudaModeCheckBox.setEnabled(false);
				}else if(e.getStateChange() == ItemEvent.DESELECTED){
					forceCudaModeCheckBox.setEnabled(true);
					forceNonCudaModeCheckBox.setEnabled(true);
				}
			}

		});
		okButton.setActionCommand("Ok");
		okButton.addActionListener(this);
		cancelButton.setActionCommand("Cancel");
		cancelButton.addActionListener(this);

		// Grupo de botões
		ButtonGroup bgroup = new ButtonGroup();
		bgroup.add(forceCudaModeCheckBox);
		bgroup.add(forceNonCudaModeCheckBox);

		Box horizontal = new Box(BoxLayout.X_AXIS);
		horizontal.setAlignmentX(LEFT_ALIGNMENT);
		horizontal.add(nonForceModeLabel);
		horizontal.add(nonForceModeCheckBox);
		vertical.add(horizontal);

		horizontal = new Box(BoxLayout.X_AXIS);
		horizontal.setAlignmentX(LEFT_ALIGNMENT);
		horizontal.add(forceCudaModeLabel);
		horizontal.add(forceCudaModeCheckBox);
		vertical.add(horizontal);

		horizontal = new Box(BoxLayout.X_AXIS);
		horizontal.setAlignmentX(LEFT_ALIGNMENT);
		horizontal.add(forceNonCudaModeLabel);
		horizontal.add(forceNonCudaModeCheckBox);		
		vertical.add(horizontal);


		switch(mode){
		case Hunter.FORCE_CUDA_MODE:
			forceCudaModeCheckBox.setSelected(true);
			break;
		case Hunter.FORCE_NONCUDA_MODE:
			forceNonCudaModeCheckBox.setSelected(true);
			break;
		case Hunter.NON_FORCE_MODE:
			nonForceModeCheckBox.setSelected(true);
			break;
		}

		panel.add(vertical,BorderLayout.CENTER);

		horizontal = new Box(BoxLayout.X_AXIS);
		Component horizontalGlue = Box.createHorizontalGlue();
		horizontal.add(horizontalGlue);
		horizontal.setAlignmentX(CENTER_ALIGNMENT);
		horizontal.add(okButton);
		horizontal.add(cancelButton);
		panel.add(horizontal,BorderLayout.SOUTH);

		this.getContentPane().setLayout(new BorderLayout());		
		this.getContentPane().add(panel,BorderLayout.CENTER);
		setSize(450, 150);
		setLocationByPlatform(true);
		setLocationRelativeTo(parent);
	}

	@Override
	public void actionPerformed(ActionEvent e) {
		if(e.getActionCommand().equals("Ok")){
			h.setMode(mode);
			Drawer.getSummaryDrawer().update(null, null);
			this.dispose();
		}else if(e.getActionCommand().equals("Cancel")){
			this.dispose();
		}
	}

}
