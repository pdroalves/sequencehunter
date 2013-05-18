package auxiliares;

import java.awt.Graphics;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.beans.PropertyChangeEvent;

import javax.swing.JComponent;
import javax.swing.JLayer;
import javax.swing.JPanel;
import javax.swing.plaf.LayerUI;

public class WaitLayerHandle extends LayerUI<JPanel> implements ActionListener {

	private WaitLayerUI layerUi;
	
	public WaitLayerHandle(){
		this.reset();
	}
	
	public WaitLayerUI getLayerUI() {
		return layerUi;
	}

	public void setLayerUI(WaitLayerUI layerUI) {
		this.layerUi = layerUI;
	}
	
	public void reset(){
		this.setLayerUI(new WaitLayerUI());
	}
	
	public void start(){
		layerUi.start();
	}
	
	public void stop(){
		layerUi.stop();
	}

	@Override
	public void actionPerformed(ActionEvent arg0) {
		layerUi.actionPerformed(arg0);
	}
	
	@Override
	public void paint (Graphics g, JComponent c) {
		layerUi.paint(g, c);
	}
	
	@Override
	public void applyPropertyChange(PropertyChangeEvent pce, JLayer l) {
		layerUi.applyPropertyChange(pce, l);
	}

}
