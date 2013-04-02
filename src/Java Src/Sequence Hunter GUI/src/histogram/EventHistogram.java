package histogram;
import histogram.SimpleHistogramPanel;

import java.awt.Color;
import java.util.ArrayList;
import java.util.Iterator;

import javax.swing.BorderFactory;
import javax.swing.border.BevelBorder;

import myTypeData.GenType;


public class EventHistogram {
	private SimpleHistogramPanel dp;
	private ArrayList data;
	private int MaxBars = 100;

	public EventHistogram() {
		dp = new SimpleHistogramPanel();
		dp.setBackground(new Color(255, 255, 255));
		dp.setBorder(BorderFactory.createBevelBorder(BevelBorder.RAISED));
		data = new ArrayList<GenType>();
		dp.setVisible(false);
	}

	public void setBarNames(boolean b){
		dp.setBarNames(b);
	}

	public void addType(GenType e) {
		data.add(e);
		checkVisibleStatus();
	}     

	public void addTypeSet(ArrayList al){
		Iterator<GenType> iterator = al.iterator();
		int added = 0;
		while(iterator.hasNext() && added < MaxBars){
			data.add(iterator.next());
		}
		checkVisibleStatus();
	}

	private void checkVisibleStatus(){
		if(data != null){
			if(data.size() > 0){
				dp.setVisible(true);
			}else{
				dp.setVisible(false);
			}
		}else{
			dp.setVisible(false);
		}
	}

	public SimpleHistogramPanel getPanel(){
		return dp;
	}

	public void commit(){
		dp.setData(data);
	}

	public void enableLinearize(boolean b){
		dp.enableLinearize(b);
	} 

	public ArrayList getData() {
		return data;
	}

} 