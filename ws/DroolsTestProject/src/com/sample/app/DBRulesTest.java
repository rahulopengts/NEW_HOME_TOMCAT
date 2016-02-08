package com.sample.app;
 
import java.io.File;

import org.drools.runtime.rule.FactHandle;
import org.junit.Test;
import org.kie.api.io.ResourceType;
import org.kie.internal.KnowledgeBase;
import org.kie.internal.builder.KnowledgeBuilder;
import org.kie.internal.builder.KnowledgeBuilderError;
import org.kie.internal.builder.KnowledgeBuilderFactory;
import org.kie.internal.io.ResourceFactory;
import org.kie.internal.runtime.StatefulKnowledgeSession;

  
public class DBRulesTest {
 
    private FactHandle serverHandle;
 
    @Test
    public void memoryTest() throws InterruptedException {
 
        final StatefulKnowledgeSession ksession = createKnowledgeSession();
        Event myevent = new Event();
        ksession.setGlobal("event", myevent);
 
        final Server node1 = new Server(1500,"server1");
          
 
        new Thread(new Runnable() {
            public void run() {
                ksession.fireUntilHalt();
            }
        }).start();
 
        serverHandle = ksession.insert(node1);
         
        int memory = 1500;
        for (int i=0;i<5;i++){
            Thread.sleep(6000);
            memory = memory - 200;
        
            node1.setMemory(memory);
            ksession.update(serverHandle, node1);
             
        }
 
        ksession.halt();
        System.out.println(myevent.getEvents());
    }
 
    private StatefulKnowledgeSession createKnowledgeSession() {
        //KnowledgeBuilder kbuilder = KnowledgeBuilderFactory.newKnowledgeBuilder();
//        kbuilder.add(new ClassPathResource("myrule.drl", getClass()), ResourceType.DRL);
    	KnowledgeBuilder kbuilder = KnowledgeBuilderFactory.newKnowledgeBuilder();
    	String fileLoc	=	"D:\\Home_Auto\\NEW_HOME_TOMCAT\\ws\\DroolsTestProject\\conf\\Sample.drl";
	    File file	=	new File(fileLoc);	
	    kbuilder.add( ResourceFactory.newFileResource(file),ResourceType.DRL);
	    
        if (kbuilder.hasErrors()) {
            if (kbuilder.getErrors().size() > 0) {
                for (KnowledgeBuilderError kerror : kbuilder.getErrors()) {
                    System.err.println(kerror);
                }
            }
        }
 
        KnowledgeBase kbase = kbuilder.newKnowledgeBase();
        StatefulKnowledgeSession ksession = kbase.newStatefulKnowledgeSession();
        return ksession;
    }
 
}