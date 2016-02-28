package org.atmosphere.sample;

import java.io.IOException;
import java.util.Set;
import java.util.concurrent.CopyOnWriteArraySet;
import java.util.concurrent.atomic.AtomicInteger;

import javax.websocket.OnClose;
import javax.websocket.OnError;
import javax.websocket.OnMessage;
import javax.websocket.OnOpen;
import javax.websocket.Session;
import javax.websocket.server.ServerEndpoint;

//import org.apache.juli.logging.Log;
//import org.apache.juli.logging.LogFactory;

//import util.HTMLFilter;

//@ServerEndpoint("/echo")
@ServerEndpoint(value = "/examples/websocket/chat")
public class WebSocketDemo {
    //private static final Log log = LogFactory.getLog(ChatAnnotation.class);

    private static final String GUEST_PREFIX = "Guest";
    private static final AtomicInteger connectionIds = new AtomicInteger(0);
    private static final Set<WebSocketDemo> connections =
            new CopyOnWriteArraySet<WebSocketDemo>();

    private final String nickname;
    private Session session;

    public WebSocketDemo() {
        nickname = GUEST_PREFIX + connectionIds.getAndIncrement();
    }


    @OnOpen
    public void start(Session session) {
    	System.out.println("-Start->");
        this.session = session;
        connections.add(this);
        String message = String.format("* %s %s", nickname, "has joined.");
        System.out.println("-Start->Message->"+message);
        broadcast(message);
    }


    @OnClose
    public void end() {
    	System.out.println("-end->"+this);
        connections.remove(this);
        String message = String.format("* %s %s",
                nickname, "has disconnected.");
        broadcast(message);
    }


    @OnMessage
    public void incoming(String message) {
    	try{
    	System.out.println("-inComing->message->"+message);
        // Never trust the client
        String filteredMessage = String.format("%s: %s",
                nickname, message.toString());
        //broadcast(filteredMessage);
        this.session.getBasicRemote().sendText(filteredMessage);
    	} catch (IOException e){
    		e.printStackTrace();
    	}
    }




    @OnError
    public void onError(Throwable t) throws Throwable {
        System.out.println("Chat Error: " + t.toString());
    }


    private void broadcast(String msg) {
    	System.out.println("-broadcast->B");
        for (WebSocketDemo client : connections) {
        	System.out.println("-broadcast->B->"+client.session.getId());
            try {
                synchronized (client) {
                    client.session.getBasicRemote().sendText(msg);
                    //this.session.getBasicRemote().sendText(msg);
                }
            } catch (IOException e) {
                System.out.println("Chat Error: Failed to send message to client");
                connections.remove(client);
                try {
                    client.session.close();
                } catch (IOException e1) {
                    // Ignore
                }
                String message = String.format("* %s %s",
                        client.nickname, "has been disconnected.");
                broadcast(message);
            }
        }
    }

}