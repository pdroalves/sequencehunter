//    This file is part of SimpleHistogramPanel.
//
//    SimpleHistogramPanel is free software: you can redistribute it and/or modify
//    it under the terms of the GNU Lesser General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    SimpleHistogramPanel is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public License
//    along with SimpleHistogramPanel.  If not, see <http://www.gnu.org/licenses/>.

package histogram;
import java.awt.Color;
import java.util.Random;

public abstract class HistogramBar{
	private String barName;
	private int value;
	private int x;
	private int y;
	private int width;
	private int height;
	private Color color;
	
	public HistogramBar(){
		barName = new String();
		value = 0;
		setX(0);
		setY(0);
		setWidth(0);
		setHeight(0);
		Random rand = new Random();
		float r = rand.nextFloat();
		float g = rand.nextFloat();
		float b = rand.nextFloat();	
		color = new Color(r,g,b);	
	}
	
	public HistogramBar(String n,int v,int x,int y,int width,int height){
		barName = n;
		value = v;
		setX(x);
		setY(y);
		setWidth(width);
		setHeight(height);
		Random rand = new Random();
		float r = rand.nextFloat();
		float g = rand.nextFloat();
		float b = rand.nextFloat();	
		color = new Color(r,g,b);
	}
	
	public HistogramBar(int x,int y,int width,int height,Color c){
		setX(x);
		setY(y);
		setWidth(width);
		setHeight(height);
		color = c;
	}
	
	public void setName(String n){
		barName = n;
	}
	
	public String getName(){
		return barName;
	}
	
	public void setValue(int n){
		value = n;
	}
	
	public int getValue(){
		return value;
	}
	
	public void setX(int n){
		x = n;
	}

	public void setY(int n){
		y = n;
	}
	
	public void setWidth(int width){
		this.width = width;
	}
	
	public void setHeight(int height){
		this.height = height;
	}
	
	public void setColor(Color c){
		color = c;
	}
	
	public Color getColor(){
		return color;
	}
	
	public void setRandomColor(){
		Random rand = new Random();
		float r = rand.nextFloat();
		float g = rand.nextFloat();
		float b = rand.nextFloat();	
		color = new Color(r,g,b);
	}

	public int getX() {
		return x;
	}

	public int getY() {
		return y;
	}

	public int getWidth() {
		return width;
	}

	public int getHeight() {
		return height;
	}

}
