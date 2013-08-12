package com.lnbio.gui.modules;

import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.util.Observable;
import java.util.Observer;

import javax.swing.JPopupMenu;
import javax.swing.JTable;

import com.lnbio.auxiliares.FiveCutPopUpMenu;
import com.lnbio.database.DBManager;
import com.lnbio.gui.ReportDrawer;

public class CentralCutMenuMouseAdapter extends MouseAdapter implements Observer{
	private JTable table;
	private ReportDrawer owner;
	private int tableIndex;
	private final int SEQUENCE_COLUMN_INDEX = 1;
	private DBManager dbm;
	private boolean enabled;

	public CentralCutMenuMouseAdapter(ReportDrawer owner,DBManager dbm,int tableIndex,JTable table){
		this.table = table;
		this.owner = owner;
		this.dbm = dbm;
		dbm.addObserver(this);
		if(dbm.isCustomFiveCutSupported())
			enabled = true;
		else
			enabled = false;
		this.tableIndex = tableIndex;
	}

	@Override
	public void mouseReleased(MouseEvent e) {
		launchPopUp(e);
	}

	@Override
	public void mousePressed(MouseEvent e) {
		launchPopUp(e);
	}

	private void launchPopUp(MouseEvent e){
		if(enabled){
			int r = table.rowAtPoint(e.getPoint());
			if (r >= 0 && r < table.getRowCount()) {
				table.setRowSelectionInterval(r, r);
			} else {
				table.clearSelection();
			}

			int rowindex = table.getSelectedRow();
			System.out.println(rowindex);
			if (rowindex < 0)
				return;
			if (e.isPopupTrigger() && e.getComponent() instanceof JTable ) {
				JPopupMenu popup = new FiveCutPopUpMenu(owner,dbm,String.valueOf(table.getModel().getValueAt(rowindex, SEQUENCE_COLUMN_INDEX)),tableIndex);
				popup.show(e.getComponent(), e.getX(), e.getY());
			}
		}
	}

	@Override
	public void update(Observable o, Object arg) {
		if(dbm.isCustomFiveCutSupported())
			enabled = true;
		else
			enabled = false;
	}
}

