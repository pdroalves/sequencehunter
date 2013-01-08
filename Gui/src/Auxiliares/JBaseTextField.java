package Auxiliares;
import javax.swing.JTextField;
import javax.swing.text.AttributeSet;
import javax.swing.text.BadLocationException;
import javax.swing.text.Document;
import javax.swing.text.PlainDocument;


public class JBaseTextField extends JTextField {
    public JBaseTextField(int cols) {
        super(cols);
    }

    protected Document createDefaultModel() {
	      return new BaseDocument();
    }

    static class BaseDocument extends PlainDocument {

        public void insertString(int offs, String str, AttributeSet a) throws BadLocationException {
        	// Aceita apenas as bases ACGTN e converte sempre para a forma maiuscula
        	
	          if (str == null || (!str.equals("A") && !str.equals("a") &&
	        		  			  !str.equals("C") && !str.equals("c") &&
	        		  			  !str.equals("G") && !str.equals("g") &&
	        		  			  !str.equals("T") && !str.equals("t") &&
	        		  			  !str.equals("N") && !str.equals("n"))){
	        	  return;
	          }
	          char[] upper = str.toCharArray();
	          for (int i = 0; i < upper.length; i++) {
	        	  upper[i] = Character.toUpperCase(upper[i]);
	          }
	          super.insertString(offs, new String(upper), a);
	      }
    }
}
