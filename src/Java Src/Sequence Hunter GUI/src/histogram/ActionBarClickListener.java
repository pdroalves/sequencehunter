package histogram;

import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.util.ArrayList;
import java.util.Iterator;


public class ActionBarClickListener implements MouseListener {

	private ArrayList<HistogramBar> data;
	private SimpleHistogramPanel shp;

	public ActionBarClickListener(SimpleHistogramPanel s){
		shp = s;
	}

	public ActionBarClickListener(ArrayList<HistogramBar> d,SimpleHistogramPanel s){
		data = d;
		shp = s;
	}	

	public void setData(ArrayList<HistogramBar> d){
		data = d;
	}

	@Override
	public void mouseClicked(MouseEvent e) {
		int mouseX = e.getX();
		int mouseY = e.getY();

		// Check if (mouseX,mouseY) is over some bar
		if(data != null){
			Iterator<HistogramBar> iterator = data.iterator();
			while(iterator.hasNext()){
				HistogramBar b = iterator.next();
				int x1 = b.getX();
				int x2 = x1 + b.getWidth();
				int y1 = b.getY();
				int y2 = y1 + b.getHeight();
				if(mouseX >= x1 && 
						mouseX <= x2 &&
						mouseY >= y1 && 
						mouseY <= y2){
					shp.barClicked(b);
				}
			}
		}
	}

	@Override
	public void mousePressed(MouseEvent e) {
		// TODO Auto-generated method stub

	}

	@Override
	public void mouseReleased(MouseEvent e) {
		// TODO Auto-generated method stub

	}

	@Override
	public void mouseEntered(MouseEvent e) {
		// TODO Auto-generated method stub

	}

	@Override
	public void mouseExited(MouseEvent e) {
		// TODO Auto-generated method stub

	}

}
