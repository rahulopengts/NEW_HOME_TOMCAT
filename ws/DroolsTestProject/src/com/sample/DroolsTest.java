package com.sample;

//import static org.junit.Assert.fail;

import java.io.File;
import java.io.IOException;
import java.io.StringReader;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.kie.api.definition.KiePackage;
import org.kie.api.definition.rule.Rule;
import org.kie.api.io.ResourceType;
import org.kie.api.runtime.StatelessKieSession;
import org.kie.internal.KnowledgeBase;
import org.kie.internal.KnowledgeBaseFactory;
import org.kie.internal.builder.KnowledgeBuilder;
import org.kie.internal.builder.KnowledgeBuilderError;
import org.kie.internal.builder.KnowledgeBuilderErrors;
import org.kie.internal.builder.KnowledgeBuilderFactory;
import org.kie.internal.definition.KnowledgePackage;
import org.kie.internal.io.ResourceFactory;
import org.kie.internal.runtime.StatefulKnowledgeSession;
import org.kie.internal.runtime.StatelessKnowledgeSession;

import com.google.common.base.Charsets;
import com.google.common.io.Files;

//import org.testng.annotations.Test;

/**
 * This is a sample class to launch a rule.
 */
public class DroolsTest {

	private KnowledgeBase base = KnowledgeBaseFactory.newKnowledgeBase();
	
	public final static String packageName = "com.sample";
	
	public static void main (String[] s){
		DroolsTest r	=	new DroolsTest();
        //r.testFireAfterRemoveRule();
        r.testFireAfterRemoveRuleWithFile();
		//r.testStatelessRules_new("");
		//r.testDrools();
	}

	  public void testFireAfterRemoveRuleWithFile() {
			String fileLoc	=	"D:\\Home_Auto\\NEW_HOME_TOMCAT\\ws\\DroolsTestProject\\conf\\hashmap.drl";
			String drlResourcesPaths[]	=	{fileLoc};
			System.out.println("\nTIME->1->"+System.currentTimeMillis());
			StatelessKieSession session = TestUtil.createStatelessKieSession(fileLoc);
	        Map<String, Object> fact = new HashMap<String, Object>();
	        fact.put("type", "Cinema");
	        fact.put("type3", "rahul2");
	        session.execute(fact);
//	        this.deleteRule("rule1");
	        session.execute(fact);
	        System.out.println("\nTIME->1->"+System.currentTimeMillis());
	    }	  
	  

	public void testStatelessRule(){
		//String fileLoc	=	"Sample.drl";
		String fileLoc	=	"D:\\Home_Auto\\NEW_HOME_TOMCAT\\ws\\DroolsTestProject\\conf\\Sample.drl";
		String drlResourcesPaths[]	=	{fileLoc};
		System.out.println("\nTIME->1->"+System.currentTimeMillis());
		StatelessKieSession session = TestUtil.createStatelessKieSession(fileLoc);
		System.out.println("\nTIME->2->"+System.currentTimeMillis()+"->session->"+session);
        Message message = new Message();
        message.setMessage("Hello WorldTT");
        message.setStatus(Message.HELLO);
        session.execute(Arrays.asList(message));

        //session = TestUtil.createStatelessKieSession(drlResourcesPaths);
        message = new Message();
        message.setMessage("Hello WorldRRR");
        message.setStatus(Message.GOODBYE);
        session.execute(Arrays.asList(message));
        
        
        String fileLoc2	=	"D:\\Home_Auto\\NEW_HOME_TOMCAT\\ws\\DroolsTestProject\\conf\\Sample2.drl";
		StatelessKieSession session1 = TestUtil.createStatelessKieSession(fileLoc2);
		System.out.println("\nTIME->2->"+System.currentTimeMillis()+"->session2->"+session1);
        Message message1 = new Message();
        message1.setMessage("Hello WorldTT");
        message1.setStatus(Message.HELLO);
        session1.execute(Arrays.asList(message1));
	}
	  public void testRemoveHasSameConElement() {
	        // DROOLS-891
	        String packageName = "com.sample";
	        String rule1 = "package " + packageName + ";" +
	                       "import java.util.Map; \n" +
	                       "rule 'rule1' \n" +
	                       "when \n" +
	                       " Map(this['type'] == 'Goods' && this['brand'] == 'a') \n" +
	                       //" Map(this['type'] == 'Goods' && this['category'] == 'b') \n" +
	                       "then \n" +
	                       "System.out.println('test rule 1'); \n"+
	                       "end";

	        KnowledgeBuilder kbuilder = KnowledgeBuilderFactory.newKnowledgeBuilder();

	        kbuilder.add( ResourceFactory.newByteArrayResource( rule1.getBytes() ), ResourceType.DRL );
	        if ( kbuilder.hasErrors() ) {
	        	System.out.println("\n Error");
	            //fail( kbuilder.getErrors().toString() );
	        }
	        KnowledgeBase kbase = KnowledgeBaseFactory.newKnowledgeBase();
	        kbase.addKnowledgePackages( kbuilder.getKnowledgePackages() );
	        kbase.removeKnowledgePackage(packageName);
	        StatelessKnowledgeSession session = kbase.newStatelessKnowledgeSession();
	        Map map	=	new HashMap();
	        map.put("type", "Goods");
	        map.put("brand", "a");
	        session.execute(Arrays.asList(map));
	        System.out.println("\n Done");
	    }

	  public void testFireAfterRemoveRule() {
	        // DROOLS-893
	        String rule1Name = "rule1";
	        String rule2Name = "rule2";

	        String rule1 =  "rule " + rule1Name + " \n" +
	                        "when \n" +
	                        " Map(  this['type'] == 'Goods'  )" +
	                        " and " +
	                        " Map(  this['type'] == 'Cinema'  )" +
	                        "then \n" +
	                        " System.out.println('test in rule1'); \n"+
	                        "end";

	        String rule2 =  "rule " + rule2Name + " \n" +
	                        "when \n" +
	                        " Map(  this['type'] == 'Goods'  )" +
	                        " and " +
	                        " Map(  this['type'] == 'Cinema'  )" +
	                        "then \n" +
	                        " System.out.println('test in rule2'); \n"+
	                        "end";

	        Map<String, Object> fact = new HashMap<String, Object>();
	        fact.put("type", "Cinema");
	        fact.put("type3", "rahul");
	        StatelessKieSession session = base.newStatelessKieSession();

	        this.addRuleToEngine(rule1);
	        session.execute(fact);

	        this.addRuleToEngine(rule2);
	        session.execute(fact);

	        this.deleteRule(rule1Name);

	        session.execute(fact);
	    }	  
	  
	  
	  
    public void testDrools() {
        try {
            // load up the knowledge base
            KnowledgeBase kbase = readKnowledgeBase();
            StatefulKnowledgeSession ksession = kbase.newStatefulKnowledgeSession();
            //StatelessKnowledgeSession ksession = kbase.newStatelessKnowledgeSession();
            // go !
            Message message = new Message();
            message.setMessage("Hello World");
            message.setStatus(Message.HELLO);
            ksession.insert(message);
            ksession.fireAllRules();
        } catch (Throwable t) {
            t.printStackTrace();
        }
    }

    private KnowledgeBase readKnowledgeBase() throws Exception {
        KnowledgeBuilder kbuilder = KnowledgeBuilderFactory.newKnowledgeBuilder();
        kbuilder.add(ResourceFactory.newClassPathResource("Sample.drl"), ResourceType.DRL);
        KnowledgeBuilderErrors errors = kbuilder.getErrors();
        
        if (errors.size() > 0) {
            for (KnowledgeBuilderError error: errors) {
                System.err.println(error);
            }
            throw new IllegalArgumentException("Could not parse knowledge.");
        }
        KnowledgeBase kbase = KnowledgeBaseFactory.newKnowledgeBase();
        kbase.addKnowledgePackages(kbuilder.getKnowledgePackages());
        return kbase;
    }
    
    //****************************************************NEW*************************************************************************************//
//    public void testStatelessRules_new(String fileContent){
//        
//            // DROOLS-893
//            String rule1Name = "rule1";
//            String rule2Name = "rule2";
//
//            String rule1 =  "rule " + rule1Name + " \n" +
//                            "when \n" +
////                            " Map(  this['type'] == 'Goods'  )" +
////                            " and " +
//                            " Map(  this['type'] == 'Cinema'  )" +
//                            "then \n" +
//                            " System.out.println('test in rule1'); \n"+
//                            "end";
//
//            String rule2 =  "rule " + rule2Name + " \n" +
//                            "when \n" +
//                            " Map(  this['type'] == 'Goods'  )" +
//                            " and " +
//                            " Map(  this['type'] == 'Cinema'  )" +
//                            "then \n" +
//                            " System.out.println('test in rule222'); \n"+
//                            "end";
//
//            
//            String fileCon	=	getFileString("Sample.drl");
//            
//            System.out.println("\fileCon->"+fileCon);
//            
//            Map<String, Object> fact = new HashMap<String, Object>();
//            fact.put("type", "Cinema");
//
//            StatelessKieSession session = base.newStatelessKieSession();
//            //System.out.println("\nSession->"+session);
//            //this.addRuleToEngine(rule1);
//            //session.execute(fact);
//            this.addRuleToEngine(fileCon);
//            List cmds = new ArrayList<>();
//            Message message = new Message();
//            message.setMessage("Hello World");
//            message.setStatus(Message.HELLO);
////            cmds.add( CommandFactory.newInsert( message ) );
////            session.execute(message);
//            
//            session.execute(Arrays.asList(message));
//            
//            //base = KnowledgeBaseFactory.newKnowledgeBase();
//            //StatelessKieSession session1 = base.newStatelessKieSession();
//            //System.out.println("\nSession->"+base);
//            
//    }
    
    public boolean deleteRule(String name) {
        this.base.removeRule(packageName, name);
        
        for(KiePackage kp : this.base.getKiePackages())
            for( Rule r : kp.getRules())
                System.out.println(r.getName()+" "+r.getPackageName());
        return true;

    }

    
    private boolean loadRule(String rule)  {
        String prefix = getPrefix();
        prefix += rule;

        KnowledgeBuilder builder = KnowledgeBuilderFactory.newKnowledgeBuilder();
        builder.add( ResourceFactory.newReaderResource( new StringReader( prefix ) ), ResourceType.DRL);
        Collection<KnowledgePackage> pkgs = this.buildKnowledge(builder);
        this.addKnowledgeToBase(pkgs);

        
//        String myRule = "import hellodrools.Message rule \"Hello World 2\" when message:Message (type==\"Test\") then System.out.println(\"Test, Drools!\"); end";
//        Resource myResource = ResourceFactory.newReaderResource((Reader) new StringReader(myRule));
//        kbuilder.add(myResource, ResourceType.DRL);
        
        
        return true;
    }

    public boolean addRuleToEngine(String rule)  {
        this.loadRule(rule);
        return true;
    }
    
    private Collection<KnowledgePackage> buildKnowledge(KnowledgeBuilder builder)  {
        if ( builder.hasErrors() ) {
            //fail( builder.getErrors().toString() );
        }
        return builder.getKnowledgePackages();
    }

    private void addKnowledgeToBase(Collection<KnowledgePackage> pkgs) {
        this.base.addKnowledgePackages( pkgs );
    }

    public String getPrefix() {
        return "package "+packageName+" \n"+
               "import java.util.Map;\n"+
               "import java.util.HashMap;\n"+
               "import org.slf4j.Logger;\n"+
               "import java.util.Date;\n"+

               "declare Counter \n"+
               "@role(event)\n"+
               " id : int \n"+
               "\n"+
               "end\n\n";
    }


    public String getFileString(String fileName) {
    	String fileContent	=	null;
    	try{
    	String fileLoc	=	"D:\\Home_Auto\\NEW_HOME_TOMCAT\\ws\\DroolsTestProject\\conf";
    	fileLoc	=	fileLoc	+ File.separator+fileName;
    	
    	File file	=	new File(fileLoc);
    	fileContent	=	Files.toString(file,Charsets.UTF_8);
    	} catch (IOException e){
    		e.printStackTrace();
    		
    	}
    	return fileContent;
    	
    }
    
}