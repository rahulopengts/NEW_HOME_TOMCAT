package org.openhab.core.drools.sessionholder;

import java.io.File;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.io.Reader;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

import org.drools.compiler.compiler.DrlParser;
import org.drools.compiler.compiler.DroolsParserException;
import org.drools.compiler.lang.descr.PackageDescr;
import org.drools.compiler.lang.descr.PatternDescr;
import org.drools.compiler.lang.descr.RuleDescr;
import org.kie.api.KieBase;
import org.kie.api.KieBaseConfiguration;
import org.kie.api.KieServices;
import org.kie.api.builder.KieBuilder;
import org.kie.api.builder.KieFileSystem;
import org.kie.api.builder.Message.Level;
import org.kie.api.builder.ReleaseId;
import org.kie.api.io.ResourceType;
import org.kie.api.runtime.KieContainer;
import org.kie.api.runtime.KieSessionConfiguration;
import org.kie.api.runtime.StatelessKieSession;
import org.kie.internal.KnowledgeBase;
import org.kie.internal.KnowledgeBaseFactory;
import org.kie.internal.builder.KnowledgeBuilder;
import org.kie.internal.builder.KnowledgeBuilderFactory;
import org.kie.internal.io.ResourceFactory;
import org.kie.internal.runtime.StatefulKnowledgeSession;
import org.openhab.core.drools.dto.DimmerDroolsDTO;
import org.openhab.core.drools.dto.IDroolsDTO;
import org.openhab.core.library.types.DecimalType;
import org.openhab.core.library.types.IncreaseDecreaseType;
import org.openhab.core.types.State;
import org.openhab.core.types.Type;



public class DroolsSessionDataHolder {
	private KieBase kbase = null;
    
	private StatefulKnowledgeSession kStatefulSession =	null;
	
			
	
	int ITEMSTARTINDEX	=	5;
	int COMMANDTYPESTARTINDEX	=	5;
	
	public KieBase getKbase() {
		return kbase;
	}

	public IDroolsDTO buildFacts(Type type,State state){
		IDroolsDTO	dto	=	null;
		if(type instanceof IncreaseDecreaseType){
			dto	=	new DimmerDroolsDTO();
			IncreaseDecreaseType	typeCommand	=	(IncreaseDecreaseType)type;
			if(state instanceof DecimalType){
				System.out.println("\nDroolsSessionDataHolder->buildFact->Setting");
				((DimmerDroolsDTO)dto).setItemState(((DecimalType)state).toString());
				((DimmerDroolsDTO)dto).setValue(((DecimalType)state).intValue());
				System.out.println("\nDroolsSessionDataHolder->buildFact->Setting-Value+"+((DecimalType)state).intValue());
				dto.setCommandName(typeCommand.toString());
				
			}
			
		}
		return dto;
	}
	public StatelessKieSession createStatelessKieSession(String... drlResourcesPaths) {
        KieServices ks = KieServices.Factory.get();
        KieContainer kcontainer = createKieContainer(ks, drlResourcesPaths);
        
        // Configure and create the KieBase
        KieBaseConfiguration kbconf = ks.newKieBaseConfiguration();
        //KieBase kbase = kcontainer.newKieBase(kbconf);
        kbase = kcontainer.newKieBase(kbconf);
        // Configure and create the KieSession
        KieSessionConfiguration ksconf = ks.newKieSessionConfiguration();
        
        return kbase.newStatelessKieSession(ksconf);
    }
    
    private KieContainer createKieContainer(KieServices ks, String... drlResourcesPaths) {
        // Create the in-memory File System and add the resources files to it
        KieFileSystem kfs = ks.newKieFileSystem();
        for (String path : drlResourcesPaths) {
            //kfs.write(ResourceFactory.newClassPathResource(path));
        	String fileLoc	=	drlResourcesPaths[0];//"D:\\Home_Auto\\NEW_HOME_TOMCAT\\ws\\DroolsTestProject\\conf";
        	//fileLoc	=	fileLoc	+ File.separator+"Sample.drl";
        	File file	=	new File(fileLoc);
        	//validateRuleFile(file);
            kfs.write(ResourceFactory.newFileResource(file));
        }
        // Create the builder for the resources of the File System
        KieBuilder kbuilder = ks.newKieBuilder(kfs);
        // Build the Kie Bases
        kbuilder.buildAll();
        // Check for errors
        if (kbuilder.getResults().hasMessages(Level.ERROR)) {
            throw new IllegalArgumentException(kbuilder.getResults().toString());
        }
        // Get the Release ID (mvn style: groupId, artifactId,version)
        ReleaseId relId = kbuilder.getKieModule().getReleaseId();
        // Create the Container, wrapping the KieModule with the given ReleaseId
        return ks.newKieContainer(relId);
    }

	public PackageDescr getRulePackageDescr(File file) {
		FileInputStream fis = null;
		PackageDescr packageDescr = null;
		
		try{
			fis = new FileInputStream(file);	
		} catch (Exception e){
			e.printStackTrace();
		}
		try{
			//File file = new File("D:\\Home_Auto\\NEW_HOME_TOMCAT\\ws\\DroolsTestProject\\conf\\Sample.drl");
			
			
			
			final Reader reader = new InputStreamReader( fis  );
	        DrlParser parser = new DrlParser();
	        
	        packageDescr = parser.parse(true,reader);
	        RuleDescr r = packageDescr.getRules().get( 1 );
	        PatternDescr pd = (PatternDescr) r.getLhs().getDescrs().get( 0 );        
	        System.out.println("\n Text:"+r.getName());
	     } catch (DroolsParserException e){
			e.printStackTrace();
			return null;
		 }
		return packageDescr;
	}

	private String[] parseRuleInfo(String strRuleDescr){
		String strArray[]	=	null;
		if(strRuleDescr!=null){
			strArray	=	strRuleDescr.split(",");
			for(int i=0;i<strArray.length;i++){
				if(i==0){
					strArray[i]	=	strArray[i].substring(ITEMSTARTINDEX);
				}
				if(i==1){
					strArray[i]	=	strArray[i].substring(COMMANDTYPESTARTINDEX);
				}
				//System.out.println("\nParseRuleInfo :"+i+":-:"+strArray[i]);
			}
		}
		return strArray;
	}
	
	public Map<String,ArrayList<String>> populateRulesTriggerInfo(PackageDescr packageDescr){
		Map<String,ArrayList<String>>	itemRulesMap	=	new HashMap<>();
        List<RuleDescr>	listRuleDescr =	packageDescr.getRules();
        Iterator<RuleDescr>	listRuleDescrIterator	=	listRuleDescr.iterator();
        while(listRuleDescrIterator.hasNext()){
        	ArrayList<String>	ruleTypeList	=	new ArrayList<String>();
        	RuleDescr	ruleDescr	=	listRuleDescrIterator.next();
        	String strRuleDescr	=	ruleDescr.getName();
        	System.out.println("\nParseRuleInfo :String for Parsing "+strRuleDescr);
        	String ruleInfo[]	=	parseRuleInfo(strRuleDescr);
				if(itemRulesMap.containsKey(ruleInfo[0])){
					ruleTypeList	=	itemRulesMap.get(ruleInfo[0]);
					ruleTypeList.add(ruleInfo[1]);
				} else {
					ruleTypeList.add(ruleInfo[1]);
				}
			itemRulesMap.put(ruleInfo[0],ruleTypeList);
        }
        
		return itemRulesMap;
	}

	KnowledgeBase kStateFulBase = KnowledgeBaseFactory.newKnowledgeBase();
	
    public void createStatefulSession(String fileName){
    	KnowledgeBuilder kbuilder = KnowledgeBuilderFactory.newKnowledgeBuilder();
    	String fileLoc	=	fileName;//"D:\\Home_Auto\\NEW_HOME_TOMCAT\\ws\\DroolsTestProject\\conf\\Sample.drl";
	    File file	=	new File(fileLoc);	
	    kbuilder.add( ResourceFactory.newFileResource(file),ResourceType.DRL);
	    //assertFalse( kbuilder.hasErrors() );
	    if (kbuilder.hasErrors() ) {
	        System.out.println( kbuilder.getErrors() );
	    }
	    kStateFulBase = KnowledgeBaseFactory.newKnowledgeBase();
	    kStateFulBase.addKnowledgePackages( kbuilder.getKnowledgePackages() );
	    //kStatefulSession = kStateFulBase.newStatefulKnowledgeSession();
	    //return kStatefulSession;
    }
    
    public StatefulKnowledgeSession getStatefulSession(){
    	kStatefulSession = kStateFulBase.newStatefulKnowledgeSession();
    	return kStatefulSession;
    }
    
    public void insertObject(IDroolsDTO idroolDTO){
    	kStatefulSession.insert( idroolDTO );
    }
    
    public void insertHashMap(Map map){
    	kStatefulSession.insert( map );
    }

    public void executeStatefulRule(){
    	kStatefulSession.fireAllRules();
    }

    
    public void disposeSession(){
    	kStatefulSession.dispose();
    }
    
    
}
