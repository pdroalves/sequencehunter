import javax.swing.SwingUtilities;
import Gui.Drawer;

public class Principal {

	public static void main(String[] args) {
		SwingUtilities.invokeLater(new Runnable() {
			public void run(){
				new Drawer();
			}
		});
	}
}
