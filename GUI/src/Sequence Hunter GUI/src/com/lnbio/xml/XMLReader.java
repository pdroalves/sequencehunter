package com.lnbio.xml;

import java.io.IOException;
import java.io.InputStream;
import java.util.HashMap;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NodeList;
import org.xml.sax.SAXException;

import com.lnbio.gui.Drawer;


public class XMLReader {
	private NodeList nList;
	public XMLReader(String filePath,String tag){

		// Carrega lingua padrao
		DocumentBuilderFactory dbFactory = DocumentBuilderFactory.newInstance();
		DocumentBuilder dBuilder;
		try {
			dBuilder = dbFactory.newDocumentBuilder();
			InputStream is = Thread.currentThread().getContextClassLoader().getResourceAsStream(filePath);
			if(is == null)
				throw new IOException("InputStream null");
			Document doc = dBuilder.parse(is);
			doc.normalize();
			
			nList = doc.getElementsByTagName(tag);		
		} catch (ParserConfigurationException | SAXException | IOException e) {
			System.out.println("Impossível carregar arquivo de linguagens. "+e.getMessage());
			Drawer.writeToLog("Error on load of language file");
			System.exit(1);
		}
	}

	public HashMap<String,String> getSet(){
		HashMap<String,String> textHM = new HashMap<String,String>();
		
		for(int i = 0;i < nList.getLength();i++){
			Element e = (Element) nList.item(i);
			textHM.put(e.getAttribute("tag"),e.getTextContent());			
		}
		
		return textHM;
	}
}
