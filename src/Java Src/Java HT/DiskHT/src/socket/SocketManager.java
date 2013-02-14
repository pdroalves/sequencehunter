//      SocketManager.java
//      
//      Copyright 2013 Pedro Alves <pdroalves@gmail.com>
//      
//		Classe responsavel por mediar a troca de mensagens entre o SH e a classe HunterDatabase
//
//		08/02/2013

package socket;

import java.net.Socket;
import java.net.ServerSocket;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.io.File;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.IOException;

import db.Event;
import db.HunterDatabase;

public class SocketManager {

	private int serverPort;
	private ServerSocket serverSock = null;
	private boolean end = false;
	private HunterDatabase db;
	private String sensoCode = "S";
	private String antisensoCode = "AS";

	public SocketManager(){
		serverPort = 9332;
		serverSock = createServerSocket(serverPort);
		db = new HunterDatabase("central", new File("centralSeq"));
	}

	public SocketManager(int n){
		serverPort = n;
		serverSock = createServerSocket(serverPort);
		db = new HunterDatabase("central", new File("centralSeq"));
	}

	public SocketManager(HunterDatabase d){
		serverPort = 9332;
		serverSock = createServerSocket(serverPort);
		db = d;
	}

	public SocketManager(int n,HunterDatabase d){
		serverPort = n;
		serverSock = createServerSocket(serverPort);
		db = d;
	}

	private ServerSocket createServerSocket(int port){
		ServerSocket sock = null;
		try {
			sock = new ServerSocket(serverPort);
		}
		catch (IOException e){
			e.printStackTrace(System.err);
		}
		return sock;
	}

	private void handleConnection(InputStream sockInput, OutputStream sockOutput) {
		int dataReceived = 0;
		boolean cincoLSupport = false;

		// Mensagens
		String helloMsg = "hello";
		String closeMsg = "bye";
		String doneMsg = "done";
		String regiaoCincolMsg = "cincolok";
		String sizeMsg = "size";
		byte[] doneMsgBytes = doneMsg.getBytes();
		byte[] helloMsgBytes = helloMsg.getBytes();
		byte[] closeMsgBytes = closeMsg.getBytes();
		byte[] regiaoCincolBytes = regiaoCincolMsg.getBytes();
		byte[] sizeBytes = sizeMsg.getBytes();

		// Patterns
		Pattern helloPattern = Pattern.compile(helloMsg);
		Pattern incomingSeqPattern = Pattern.compile("1(.+)2(.+)\\s");// Seq + Tipo
		Pattern incomingSeqWithCincoLSupportPattern = Pattern.compile("1(.+)2(.+)3(.+)\\s"); // SeqCentral + SeqCL + Tipo
		Pattern closePattern = Pattern.compile(closeMsg);
		Pattern regiaoCLPattern = Pattern.compile(regiaoCincolMsg);
		Pattern sizePattern = Pattern.compile(sizeMsg);

		while(!end) {
			byte[] buf=new byte[1024];
			int bytes_read = 0;

			try {
				// This call to read() will wait forever, until the
				// program on the other side either sends some data,
				// or closes the socket.
				bytes_read = getMsg(sockInput,buf,0,buf.length);
				sendMsg(sockOutput,doneMsgBytes,0,doneMsgBytes.length);
				sockOutput.flush();

				// If the socket is closed, sockInput.read() will return -1.
				if(bytes_read < 0) {
					System.err.println("Tried to read from socket, read() returned < 0,  Closing socket.");
					return;
				}
				String data = new String(buf, 0, bytes_read);

				Matcher incomingSeqWithCincoLSupportMatcher = incomingSeqWithCincoLSupportPattern.matcher(data);
				Matcher incomingSeqMatcher = incomingSeqPattern.matcher(data);
				Matcher helloMatcher = helloPattern.matcher(data);
				Matcher closeMatcher = closePattern.matcher(data);
				Matcher regiaoCLMatcher = regiaoCLPattern.matcher(data);
				Matcher sizeMatcher = sizePattern.matcher(data);

				if(helloMatcher.find()){
					sendMsg(sockOutput,helloMsgBytes,0,helloMsgBytes.length);
				}else if(closeMatcher.find()){
					sendMsg(sockOutput,closeMsgBytes,0,closeMsgBytes.length);
					end = true;
				}else if(regiaoCLMatcher.find()){
					cincoLSupport = true;					
				}else if(sizeMatcher.find()){
					byte[] sizeSendBytes = Integer.toString(db.size()).getBytes();
					sendMsg(sockOutput,sizeSendBytes,0,sizeSendBytes.length);
				}else if(incomingSeqWithCincoLSupportMatcher.find()){
					boolean hasData = true;
					while(hasData){

						dataReceived++;
						String seq = incomingSeqWithCincoLSupportMatcher.group(1);
						String seqCL = incomingSeqWithCincoLSupportMatcher.group(2);
						String seqTipo = incomingSeqWithCincoLSupportMatcher.group(3);
						System.out.println("Seq: "+seq+" seqCL: "+seqCL+" tipo: "+seqTipo);
						if(sensoCode.equals(seqTipo)){
							db.add(seq,seqCL, new Event(seq,1,0));
						}
						else if(antisensoCode.equals(seqTipo)){
							db.add(seq,seqCL, new Event(seq,0,1));					
						}
						hasData = incomingSeqWithCincoLSupportMatcher.find();
					}
				}else if(incomingSeqMatcher.find()){
					boolean hasData = true;
					while(hasData){

						dataReceived++;
						String seq = incomingSeqMatcher.group(1);
						if(cincoLSupport){			
							String seqCL = incomingSeqMatcher.group(2);
							String seqTipo = incomingSeqMatcher.group(3);
							if(sensoCode.equals(seqTipo)){
								db.add(seq,seqCL, new Event(seq,1,0));
							}
							else if(antisensoCode.equals(seqTipo)){
								db.add(seq,seqCL, new Event(seq,0,1));					
							}
						}else{
							String seqTipo = incomingSeqMatcher.group(2);
							if(sensoCode.equals(seqTipo)){
								db.add(seq,null, new Event(seq,1,0));
							}
							else if(antisensoCode.equals(seqTipo)){
								db.add(seq,null, new Event(seq,0,1));					
							}
						}
						hasData = incomingSeqMatcher.find();
					}
				}


				// This call to flush() is optional - we're saying go
				// ahead and send the data now instead of buffering
				// it.
				sockOutput.flush();
			}
			catch (IOException e){
				System.err.println("Exception reading from/writing to socket, e="+e);
				e.printStackTrace(System.err);
				return;
			}
			System.out.println("Seqs received: "+dataReceived);
		}

	}

	private void sendMsg(OutputStream sockOutput,byte[] b,int off,int size) throws IOException{
		sockOutput.write(b,off,size);		
	}

	private int getMsg(InputStream sockInput,byte[] b,int off,int size) throws IOException{
		int bytes_read = sockInput.read(b,off,size);
		return bytes_read;
	}

	public void waitForConnections() {
		Socket sock = null;
		InputStream sockInput = null;
		OutputStream sockOutput = null;
		while (!end) {
			try {
				// This method call, accept(), blocks and waits
				// (forever if necessary) until some other program
				// opens a socket connection to our server.  When some
				// other program opens a connection to our server,
				// accept() creates a new socket to represent that
				// connection and returns.
				sock = serverSock.accept();
				System.err.println("Have accepted new socket.");

				// From this point on, no new socket connections can
				// be made to our server until we call accept() again.

				sockInput = sock.getInputStream();
				sockOutput = sock.getOutputStream();
			}
			catch (IOException e){
				e.printStackTrace(System.err);
			}

			// Do something with the socket - read bytes from the
			// socket and write them back to the socket until the
			// other side closes the connection.
			handleConnection(sockInput, sockOutput);

			// Now we close the socket.
			try {
				System.err.println("Closing socket.");
				sock.close();
			}
			catch (Exception e){
				System.err.println("Exception while closing socket.");
				e.printStackTrace(System.err);
			}

			System.err.println("Finished with socket.");
		}
	}

}
