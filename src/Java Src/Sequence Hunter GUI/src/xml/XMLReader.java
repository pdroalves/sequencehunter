package xml;

import gui.Drawer;

import java.io.File;
import java.io.IOException;
import java.util.HashMap;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NodeList;
import org.xml.sax.SAXException;


public class XMLReader {
	private NodeList nList;
	public XMLReader(String filePath,String tag){

		// Carrega lingua padrao
		DocumentBuilderFactory dbFactory = DocumentBuilderFactory.newInstance();
		DocumentBuilder dBuilder;
		try {
			dBuilder = dbFactory.newDocumentBuilder();
			Document doc = dBuilder.parse(new File(filePath));
			doc.normalize();
			
			nList = doc.getElementsByTagName(tag);		
		} catch (ParserConfigurationException | SAXException | IOException e) {
			System.out.println("Impossível carregar arquivo de linguagens.");
			Drawer.writeToLog("Error on load of language file");
		}
	}

	public HashMap<String,String> getSet(){
		HashMap<String,String> textHM = new HashMap<String,String>();
		
		for(int i = 0;i < nList.getLength();i++){
			Element e = (Element) nList.item(i);
			textHM.put(e.getAttribute("tag"),e.getAttribute("text"));			
		}
		
		return textHM;
	}
}
