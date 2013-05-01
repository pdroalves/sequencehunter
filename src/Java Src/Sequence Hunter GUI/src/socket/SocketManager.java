//      SocketManager.java
//      
//      Copyright 2013 Pedro Alves <pdroalves@gmail.com>
//      
//		Classe responsavel por mediar a troca de mensagens entre o SH e a classe HunterDatabase
//
//		08/02/2013

package socket;

import gui.Drawer;

import java.net.Socket;
import java.net.ServerSocket;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.IOException;

public class SocketManager {

	private int serverPort;
	private ServerSocket serverSock = null;
	private boolean end = false;
	private ISocketUser isu;

	public SocketManager(ISocketUser user){
		serverPort = 9332;
		serverSock = createServerSocket(serverPort);
		isu = user;
	}

	public SocketManager(ISocketUser user,int n){
		serverPort = n;
		serverSock = createServerSocket(serverPort);
		isu = user;
	}

	private ServerSocket createServerSocket(int port){
		ServerSocket sock = null;
		try {
			sock = new ServerSocket(serverPort);
		}
		catch (IOException e){
			e.printStackTrace(System.err);
			Drawer.writeToLog("Error on connection to CLI.");
			//SummaryDrawer.huntAbort();
		}
		return sock;
	}

	private void handleConnection(InputStream sockInput, OutputStream sockOutput) {
		// Mensagens
		String data;
		String helloMsg = "hello";
		String closeMsg = "bye";
		String doneMsg = "done";
		byte[] doneMsgBytes = doneMsg.getBytes();
		byte[] helloMsgBytes = helloMsg.getBytes();
		byte[] closeMsgBytes = closeMsg.getBytes();

		// Patterns
		Pattern helloPattern = Pattern.compile(helloMsg);
		Pattern closePattern = Pattern.compile(closeMsg);

		while(!end && !isu.getKill()) {
			byte[] buf=new byte[1024];
			int bytes_read = 0;

			try {
				// This call to read() will wait forever, until the
				// program on the other side either sends some data,
				// or closes the socket.
				bytes_read = getMsg(sockInput,buf,0,buf.length);

				// If the socket is closed, sockInput.read() will return -1.
				if(bytes_read < 0) {
					System.err.println("Tried to read from socket, read() returned < 0,  Closing socket.");
					return;
				}
				data = new String(buf, 0, bytes_read);
				System.err.println("Socket said: "+data);
				
				Matcher helloMatcher = helloPattern.matcher(data);
				Matcher closeMatcher = closePattern.matcher(data);

				if(helloMatcher.find()){
					sendMsg(sockOutput,helloMsgBytes,0,helloMsgBytes.length);
				}else if(closeMatcher.find()){
					sendMsg(sockOutput,closeMsgBytes,0,closeMsgBytes.length);
					end = true;
				}else{
					sendMsg(sockOutput,doneMsgBytes,0,doneMsgBytes.length);
					isu.translate(data);
				}
							
				// This call to flush() is optional - we're saying go
				// ahead and send the data now instead of buffering
				// it.
				sockOutput.flush();
			}
			catch (IOException e){
				Drawer.writeToLog(e.getMessage());
			}
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
			
	
				// Do something with the socket - read bytes from the
				// socket and write them back to the socket until the
				// other side closes the connection.
				handleConnection(sockInput, sockOutput);
	
				// Now we close the socket.
				System.err.println("Closing socket.");
				sockInput.close();
				sockOutput.close();
				sock.close();
				serverSock.close();
			
		}catch (IOException e){
				System.err.println("Exception:");
				e.printStackTrace(System.err);
			}
		catch (NullPointerException e){
			System.err.println("Exception:");
			
		}

			System.err.println("Finished with socket.");	
	}

}
