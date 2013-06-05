package tables;

import java.awt.Component;
import java.awt.Font;

import javax.swing.JTable;
import javax.swing.table.DefaultTableCellRenderer;
import javax.swing.table.TableCellRenderer;

public class JTableRenderer implements TableCellRenderer{
	private TableCellRenderer delegate;
	
	public JTableRenderer(TableCellRenderer defaultRenderer){
		delegate = defaultRenderer;
	}
	
		@Override
		public Component getTableCellRendererComponent(JTable table,
				Object value, boolean isSelected, boolean hasFocus, int row,
				int column) {
			Component colortext = delegate.getTableCellRendererComponent(table, value, isSelected, hasFocus, row, column);
			
			colortext.setFont(new Font("Monospaced",Font.PLAIN,14));
			return colortext;
		}
}
