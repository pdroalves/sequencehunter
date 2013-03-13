package gui;

import hunt.Hunter;
import hunt.Library;

import java.awt.BorderLayout;
import java.awt.Component;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.GridBagConstraints;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.AdjustmentEvent;
import java.awt.event.AdjustmentListener;
import java.io.File;
import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import javax.swing.Box;
import javax.swing.DefaultListModel;
import javax.swing.JButton;
import javax.swing.JFileChooser;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JPanel;
import javax.swing.JProgressBar;
import javax.swing.JScrollBar;
import javax.swing.JScrollPane;
import javax.swing.JSplitPane;
import javax.swing.JTabbedPane;
import javax.swing.JTable;
import javax.swing.ListSelectionModel;
import javax.swing.table.DefaultTableCellRenderer;

import tables.JLibPreviewTableModel;
import tables.JTableRenderer;

import auxiliares.ButtonTabComponent;
import auxiliares.JBaseTextField;
import auxiliares.JTxtFileFilter;

public class SearchDrawer implements ActionListener{
	private String searchSeq;
	private JBaseTextField seqOriginal;
	private static JLabel seqBusca;
	private JButton setSeqButton;
	private JList<String> jl ;
	private DefaultListModel<String> listModel;
	private JTabbedPane libContainer;
	private JPanel jpTableList;
	private boolean emptyLibPreview;
	private int xSize = 700;
	private int ySize = 1000;
	private ArrayList<String> libs = new ArrayList<String>();
	private JPanel seqBuscaPanel;
	private JPanel seqBuscaRightPanel;
	private JPanel seqBuscaLeftPanel;
	private Hunter h;
	private JSplitPane jsp;

	public SearchDrawer(int xSize,int ySize,Hunter hunterInstance){
		this.xSize = xSize;
		this.ySize = ySize;
		seqOriginal = new JBaseTextField(25);
		seqOriginal.setMaximumSize(new Dimension(xSize*2, 30));
		seqBusca = new JLabel();
		jl = new JList<String>();
		listModel = new DefaultListModel<String>();  
		libContainer = new JTabbedPane(JTabbedPane.TOP,JTabbedPane.SCROLL_TAB_LAYOUT);	
		jpTableList = new JPanel();
		seqBuscaPanel = new JPanel();
		seqBuscaRightPanel = new JPanel(new BorderLayout());
		seqBuscaLeftPanel = new JPanel(new BorderLayout());
		drawSearchContainer();
		h = hunterInstance;
	}

	public JPanel getContainer(){
		return seqBuscaPanel;
	}

	private void drawSearchContainer(){		
		GridBagConstraints c = new GridBagConstraints();
		// Configura tab para sequencias
		seqBuscaLeftPanel.setLayout(new GridLayout(4,1));
		//Adiciona nova linha hbox
		Box hbox = Box.createHorizontalBox();
		hbox.add(new JLabel("Sequence: "));
		hbox.add(seqOriginal);
		setSeqButton = new JButton("Set");
		setSeqButton.addActionListener(this);
		hbox.add(setSeqButton);
		// Configura linha
	    c.fill = GridBagConstraints.HORIZONTAL;
	    c.weighty = 0.05;
	    c.weightx = 1;
	    c.gridx = 0;
	    c.gridy = 0;
		seqBuscaLeftPanel.add(hbox);
		
		// Adiciona nova linha hbox
		hbox = Box.createHorizontalBox();
		hbox.add(new JLabel("Target Sequence: "));
		hbox.add(seqBusca);
		hbox.setMaximumSize(new Dimension(xSize*2,20));
		// Configura linha
	    c.fill = GridBagConstraints.HORIZONTAL;
	    c.weighty = 0.05;
	    c.weightx = 1;
	    c.gridx = 0;
	    c.gridy = 1;
		seqBuscaLeftPanel.add(hbox,c);	
		
		// Nova linha com as previews
		hbox = Box.createHorizontalBox();
		hbox.add(new JLabel("Libraries loaded: "));
		// Configura linha na libsLoaded
		JScrollPane jscrlp = new JScrollPane(jl);
		jl.setModel(listModel);
		hbox.add(jscrlp);
		// Configura linha
	    c.fill = GridBagConstraints.NONE;
	    c.weighty = 0.20;
	    c.weightx = 1;
	    c.gridx = 0;
	    c.gridy = 2;
		seqBuscaLeftPanel.add(hbox,c);

		// Adiciona nova linha hbox
		hbox = Box.createHorizontalBox();
		JButton loadLib = new JButton("Load");
		loadLib.addActionListener(this);
		hbox.add(loadLib);
		JButton unloadLib = new JButton("Unload");
		unloadLib.addActionListener(this);
		hbox.add(unloadLib);
		// Configura linha
	    c.fill = GridBagConstraints.NONE;
	    c.weighty = 0.05;
	    c.weightx = 0.2;
	    c.gridx = 0;
	    c.gridy = 3;
		seqBuscaLeftPanel.add(hbox,c);
	

		// Configura linha
		drawEmptyLibsContainer();
	    seqBuscaRightPanel.add(libContainer);  
		seqBuscaPanel.setLayout(new GridLayout(1,1));
		jsp = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT,true,seqBuscaLeftPanel,seqBuscaRightPanel);
		jsp.setOneTouchExpandable(true);
	    c.fill = GridBagConstraints.BOTH;
	    c.weighty = 1;
	    c.weightx = 1;
	    c.gridx = 0;
	    c.gridy = 0;
		seqBuscaPanel.add(jsp,c);

		return;
	}

	private void drawEmptyLibsContainer(){	
		emptyLibPreview = true;
		JPanel jp = new JPanel (new BorderLayout());
		libContainer.removeAll();		
		JLabel emptyLabel = new JLabel("Add a library to start...");
		jp.add(emptyLabel,BorderLayout.CENTER);
		libContainer.add(jp);
		return;
	}

	private void loadLibPreview(String libPath){
		if(emptyLibPreview){
			libContainer.removeAll();		
			emptyLibPreview = false;
		}
		// Esse metodo deve ser chamado para carregar uma lib
		try{
			// Abre biblioteca
			Library lib = new Library(new File(libPath));
			if(!lib.canRead()){
				throw new FileNotFoundException();
			}
			
			// Inicializa container que irá guardar os dados
			JPanel jp = new JPanel(new BorderLayout());
			final JLibPreviewTableModel jltm = new JLibPreviewTableModel(lib);
			
			// Inicializa tabela que irá mostrar as sequências
			JTable jtabPreviewLibs = new JTable(jltm);
			
			// Seta configurações de render para a tabela
			DefaultTableCellRenderer indexRenderer = new DefaultTableCellRenderer();
			indexRenderer.setHorizontalAlignment(DefaultTableCellRenderer.CENTER);
			jtabPreviewLibs.getColumnModel().getColumn(0).setCellRenderer(new JTableRenderer(indexRenderer));
			DefaultTableCellRenderer seqRenderer = new DefaultTableCellRenderer();
			jtabPreviewLibs.getColumnModel().getColumn(1).setCellRenderer(new JTableRenderer(seqRenderer));
			jtabPreviewLibs.setAutoResizeMode(JTable.AUTO_RESIZE_OFF);
			jtabPreviewLibs.getColumnModel().getColumn(0).setPreferredWidth(40);
			jtabPreviewLibs.getColumnModel().getColumn(1).setPreferredWidth((int)(ySize*0.84));
			//SelectionListener listener = new SelectionListener(jtabPreviewLibs);
			jtabPreviewLibs.getSelectionModel().setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
			jtabPreviewLibs.setAutoscrolls(true);

			// Insere JTable dentro de JScrollPane
			JScrollPane jscrlpLib  = new JScrollPane(jtabPreviewLibs);
			
			// Define lazy load
			JScrollBar jsb = jscrlpLib.getVerticalScrollBar();
			jsb.addAdjustmentListener(new AdjustmentListener(){
				@Override
				public void adjustmentValueChanged(AdjustmentEvent e) {
					JScrollBar jsb = (JScrollBar) e.getSource();
					int jsbMax = jsb.getMaximum();
					int jsbPos = jsb.getValue();
					if(jsbMax*0.8 <= jsbPos){
						jltm.loadMore();
					}
				}					
			});

			// Insere o JScrollPane com a Lib carregada dentro do JPane
			//jp.add(new JLabel(Long.toString(lib.getFileSize())));
			jp.add(jscrlpLib,BorderLayout.CENTER);

			// Adiciona aba com a lib carregada
			libContainer.addTab(lib.getFilename(),jp);

			//initTabsComponents(libContainer);

			Drawer.writeToLog("File "+libPath+" has loaded.");
		}catch(FileNotFoundException e){
			Drawer.writeToLog("File "+libPath+" could not be loaded.");
		}


		return;
	}
	
	private void unloadLibPreview(int index){
		libContainer.remove(index);
		if(libContainer.getTabCount() == 0){
			drawEmptyLibsContainer();
		}
		return;
	}

	private static void initTabsComponents(JTabbedPane pane){
		if(pane != null){
			for(int i=0;i < pane.getTabCount();i++){
				pane.setTabComponentAt(i, new ButtonTabComponent(pane));
			}
		}
	}

	public static String getTargetSeq(){
		return seqBusca.getText();
	}

	@Override
	public void actionPerformed(ActionEvent ae) {
		switch(ae.getActionCommand()){
		case "Set":
			if(seqOriginal.getSelectedText() != null)
				searchSeq = seqOriginal.getSelectedText();
			else	
				searchSeq = seqOriginal.getText();
			seqBusca.setText(searchSeq);
			Drawer.writeToLog("Target sequence: " + searchSeq);
			// To-do: Atualizar highlights nas tabelas lib preview
			SummaryDrawer.setTargetSeq(searchSeq);
			libContainer.repaint();
			break;
		case "Load":
			JFileChooser jfc = new JFileChooser();
			jfc.setCurrentDirectory(new File(h.getOutput()));
			jfc.setFileFilter(new JTxtFileFilter());
			jfc.setMultiSelectionEnabled(true);
			if(jfc.showOpenDialog(null) == JFileChooser.APPROVE_OPTION){
				for(File f: jfc.getSelectedFiles()){
					String txt = f.getAbsolutePath();
					if(f.canRead()){						
						libs.add(txt);
						loadLibPreview(txt);
						SummaryDrawer.addLoadedLib(txt);
						listModel.addElement(txt);
						Drawer.writeToLog("File "+txt+" is being loaded.");
					}else{
						Drawer.writeToLog("File "+txt+" can not be read.");
					}
				}
			}
			// To-do: Atualizar highlights nas tabelas lib preview
			break;
		case "Unload":
			List<String> elements =jl.getSelectedValuesList();
			for(String ele: elements){
				int index = libs.indexOf(ele);
				unloadLibPreview(index);
				SummaryDrawer.removeLoadedLib(ele);
				libs.remove(ele);
				listModel.removeElement(ele);
				Drawer.writeToLog("File "+ele+" unloaded.");
			}
			// To-do: Atualizar highlights nas tabelas lib preview
			break;
		}
	}

}
