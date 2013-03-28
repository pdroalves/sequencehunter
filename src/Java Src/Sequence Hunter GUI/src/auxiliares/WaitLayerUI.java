package auxiliares;

import java.awt.AlphaComposite;
import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Composite;
import java.awt.Font;
import java.awt.FontMetrics;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.RenderingHints;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.beans.PropertyChangeEvent;

import javax.swing.JComponent;
import javax.swing.JLayer;
import javax.swing.JPanel;
import javax.swing.Timer;
import javax.swing.plaf.LayerUI;

import xml.TranslationsManager;

public class WaitLayerUI extends LayerUI<JPanel> implements ActionListener {
	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	private boolean mIsRunning;
	private boolean mIsFadingOut;
	private Timer mTimer;

	private int mAngle;
	private int mFadeCount;
	private int mFadeLimit = 15;
	
	private Font defaultFont = new Font("Arial", Font.BOLD, 13);

	@Override
	public void paint (Graphics g, JComponent c) {
		
		// Pega dimensoes do component
		int width = c.getWidth();
		int height = c.getHeight();

		// Paint the view.
		super.paint (g, c);

		if (!mIsRunning) {
			return;
		}

		Graphics2D g2 = (Graphics2D)g.create();

		float fade = (float)mFadeCount / (float)mFadeLimit;
		
		// Gray it out.
		Composite urComposite = g2.getComposite();
		float alpha = .5f * fade;
		if(alpha < 0)
			alpha = 0;
		if(alpha > 1)
			alpha = 1;
		g2.setComposite(AlphaComposite.getInstance(
				AlphaComposite.SRC_OVER, alpha));
		g2.fillRect(0, 0, width, height);
		g2.setComposite(urComposite);

		// Paint the wait indicator.
		// s eh o espaco entre o ponto central e o comeco das barras, alem de referencia para seu tamanho
		int s = Math.min(width, height) / 7;
		// cx e cy sao as coordenadas do ponto central
		int cx = width / 2;
		int cy = height / 2;
		
		g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING,
				RenderingHints.VALUE_ANTIALIAS_ON);
		g2.setStroke(
				new BasicStroke(s / 4, BasicStroke.CAP_ROUND, BasicStroke.JOIN_ROUND));
		g2.setPaint(Color.white);
		g2.rotate(Math.PI * mAngle / 180, cx, cy);
		
		int lineCount = 12;//Quantidade de linhas a serem tracadas
		for (int i = 0; i < lineCount; i++) {
			float scale = (11.0f - (float)i) / 11.0f;
			g2.drawLine(cx + s, cy, cx + s * 2, cy);
			g2.rotate(-Math.PI / 6, cx, cy);
			g2.setComposite(AlphaComposite.getInstance(
					AlphaComposite.SRC_OVER, scale * fade));
		}		
		return;
	}

	public void actionPerformed(ActionEvent e) {
		if (mIsRunning) {
			firePropertyChange("tick", 0, 1);
			mAngle += 3;
			if (mAngle >= 360) {
				mAngle = 0;
			}
			if (mIsFadingOut) {
				if (--mFadeCount == 0) {
					mIsRunning = false;
					mTimer.stop();
				}
			}
			else if (mFadeCount < mFadeLimit) {
				mFadeCount++;
			}
		}
	}

	public void start() {
		if (mIsRunning) {
			return;
		}

		// Run a thread for animation.
		mIsRunning = true;
		mIsFadingOut = false;
		mFadeCount = 0;
		int fps = 24;
		int tick = 1000 / fps;
		mTimer = new Timer(tick, this);
		mTimer.start();
	}

	public void stop() {
		mIsFadingOut = true;
	}

	@Override
	public void applyPropertyChange(PropertyChangeEvent pce, JLayer l) {
		if ("tick".equals(pce.getPropertyName())) {
			l.repaint();
		}
	}
}