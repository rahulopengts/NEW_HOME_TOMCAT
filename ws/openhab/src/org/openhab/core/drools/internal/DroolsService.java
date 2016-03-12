package org.openhab.core.drools.internal;

import static org.openhab.core.events.EventConstants.TOPIC_PREFIX;
import static org.openhab.core.events.EventConstants.TOPIC_SEPERATOR;

import java.io.File;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.drools.compiler.lang.descr.PackageDescr;
import org.kie.internal.runtime.StatefulKnowledgeSession;
import org.kie.api.runtime.StatelessKieSession;
import org.openhab.core.drools.dto.CommonDroolsDTO;
import org.openhab.core.drools.dto.IDroolsDTO;
import org.openhab.core.drools.sessionholder.DroolsSessionDataHolder;
import org.openhab.core.items.GenericItem;
import org.openhab.core.items.Item;
import org.openhab.core.items.ItemNotFoundException;
import org.openhab.core.items.ItemRegistry;
import org.openhab.core.items.ItemRegistryChangeListener;
import org.openhab.core.items.StateChangeListener;
import org.openhab.core.library.items.DimmerItem;
import org.openhab.core.library.items.SwitchItem;
import org.openhab.core.types.Command;
import org.openhab.core.types.EventType;
import org.openhab.core.types.State;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.common.collect.LinkedListMultimap;
import com.openhab.core.constants.CloudAppConstants;
import com.openhab.core.internal.event.dto.CloudEvent;

public class DroolsService  implements StateChangeListener , ItemRegistryChangeListener {//EventHandler,{

    private static final Logger logger = LoggerFactory.getLogger(DroolsService.class);
    private ItemRegistry itemRegistry;
    private final LinkedListMultimap<String, Object> updates = LinkedListMultimap.create();
    private DroolsScriptExecuter	droolsScriptExecuter	=	null;
    
    private StatelessKieSession	session	=	null;
    
    private StatefulKnowledgeSession	kStatefulSession	=	null;
    private Map<String, String> factHashMap	=	new HashMap<>();
	public final static String FACT_OLD_SUFFIX	=	".OLD";
	public final static String FACT_NEW_SUFFIX	=	".NEW";
	public final static String DROOLS_FILE_SUFFIX	=	".drl";
	private PackageDescr	packageDecs	=	null;
	private DroolsSessionDataHolder	droolsSessionDataHolder	=	null;
	private Map<String,ArrayList<String>>	itemRulesMap	=	new HashMap<String,ArrayList<String>>();
	private String siteName	=	null;
	private static String fileLocDir	=	"D:\\Home_Auto\\NEW_HOME_TOMCAT\\ws\\DroolsTestProject\\conf\\";
	
	boolean IS_STATELESS	=	false;
	
    public String getSiteName() {
		return siteName;
	}

    
    


	public void setSiteName(String siteName) {
		this.siteName = siteName;
	}



	public void activate() {
    	droolsSessionDataHolder	=	new DroolsSessionDataHolder();
    	droolsScriptExecuter	=	new DroolsScriptExecuter();
		String fileLoc	=	fileLocDir+siteName+DROOLS_FILE_SUFFIX;
    	File droolsFile	=	null;
		try{
			droolsFile	=	new File(fileLoc);
			if(IS_STATELESS){
				session = droolsSessionDataHolder.createStatelessKieSession(fileLoc);
			} else {
				droolsSessionDataHolder.createStatefulSession(fileLoc);
			}
			
			packageDecs	=	droolsSessionDataHolder.getRulePackageDescr(droolsFile);
			itemRulesMap	=	droolsSessionDataHolder.populateRulesTriggerInfo(packageDecs);
	        addItems(itemRegistry.getItems());
    	} catch (Exception e){
    		e.printStackTrace();
    	}
        logger.info("Dools service started");
    }
    


    private void addItems(Collection<Item> items) {
        for (Item item : items) {
            if (item instanceof GenericItem) {
                ((GenericItem) item).addStateChangeListener(this);
                System.out.println("\nDroolsService->addItems->"+item.getName());
        		State	itemState	=	item.getState();
        		factHashMap.put(item.getName(), itemState.toString());
        		
//        		factHashMap.put(item.getName()+FACT_NEW_SUFFIX, itemState.toString());
//        		factHashMap.put(item.getName()+FACT_OLD_SUFFIX, itemState.toString());
            }
        }
    }

	@Override
	public void stateChanged(Item item, State oldState, State newState) {
		// TODO Auto-generated method stub
		System.out.println("\nDroolsService->stateChanged->"+item.getName()+"->oldState->"+oldState+"->newState->"+newState);
		if(IS_STATELESS){
			factHashMap.put(item.getName()+FACT_NEW_SUFFIX, newState.toString());
			factHashMap.put(item.getName()+FACT_OLD_SUFFIX, oldState.toString());		
			
			droolsScriptExecuter.executeRule(session, itemRegistry,factHashMap);
			//factHashMap.put(item.getName()+FACT_OLD_SUFFIX, newState.toString());
			factHashMap.remove(item.getName()+FACT_NEW_SUFFIX);
			factHashMap.remove(item.getName()+FACT_OLD_SUFFIX);
		} else {
			factHashMap.put(item.getName()+FACT_NEW_SUFFIX, newState.toString());
			factHashMap.put(item.getName()+FACT_OLD_SUFFIX, oldState.toString());		
			kStatefulSession	=	droolsSessionDataHolder.getStatefulSession();	
			droolsSessionDataHolder.insertHashMap(factHashMap);
			droolsSessionDataHolder.executeStatefulRule();
			droolsSessionDataHolder.disposeSession();
			//factHashMap.put(item.getName()+FACT_OLD_SUFFIX, newState.toString());
			factHashMap.remove(item.getName()+FACT_NEW_SUFFIX);
			factHashMap.remove(item.getName()+FACT_OLD_SUFFIX);
			
		}
	}
	
	private boolean identifyFactChanges(Item item, State oldState, State newState){
		boolean fireRule	=	false;
		if(item instanceof SwitchItem){
			System.out.println("\nDroolsService->stateChanged->instanceof Swithitem");
		} else if (item instanceof DimmerItem){
			System.out.println("\nDroolsService->stateChanged->instanceof Dimmer->state instance of -> "+oldState.getClass());
		}
		return fireRule;
	}

	@Override
	public void stateUpdated(Item item, State state) {
		// TODO Auto-generated method stub
		System.out.println("\nDroolsService->stateUpdated->"+item.getName()+"->State->"+state);		
	}

	@Override
	public void allItemsChanged(Collection<String> oldItemNames) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void itemAdded(Item item) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void itemRemoved(Item item) {
		// TODO Auto-generated method stub
		
	}

	///@Override
	public void handleEvent(CloudEvent event) {
	//public void handleEvent(Event arg0) {
		// TODO Auto-generated method stub
		String itemName = (String) event.getProperty("item");
		
		String topic = event.getTopic();
		
		String[] topicParts = topic.split(TOPIC_SEPERATOR);
		System.out.println("\nDroolsService->handleEvent->topic->"+topic+"->topicParts->1->"+topicParts[0]+"->2->"+topicParts[1]);
		if(!(topicParts.length > 2) || !topicParts[0].equals(TOPIC_PREFIX)) {
			return; // we have received an event with an invalid topic
		}
		String operation = topicParts[1];
		
		if(operation.equals(EventType.COMMAND.toString())) {
			Command command = (Command) event.getProperty("command");
			if(command!=null) receiveCommand(itemName, command);
		}
		
	}

    public void setItemRegistry(ItemRegistry itemRegistry) {
        this.itemRegistry = itemRegistry;
        itemRegistry.addItemRegistryChangeListener(this);
    }

    public void unsetItemRegistry(ItemRegistry itemRegistry) {
        itemRegistry.removeItemRegistryChangeListener(this);
        this.itemRegistry = null;
    }

	public void receiveCommand(String itemName, Command command) {
		System.out.println("\nDroolsEngine->receiveCommand->1->itemName"+itemName+"->oldState->"+itemName+"->newState->"+command);
		//if(triggerManager!=null && itemRegistry!=null) {
		if(itemRegistry!=null) {
			try {
				Item item = itemRegistry.getItem(itemName);
				System.out.println("\nRuleEngine->receiveCommand->2->itemName"+itemName+"->command->"+command+"newState->"+item.getState()+"->Class->"+item.getState().getClass());
				State	itemState	=	item.getState();
				if(validateRuleExecution(itemName,command)){
					if(IS_STATELESS){
						System.out.println("\nDroolsService->receiveCommand->Firing->"+itemState);		
						factHashMap.put(item.getName()+".COMMAND", command.toString());
						droolsScriptExecuter.executeRule(session, itemRegistry,factHashMap);
						factHashMap.remove(item.getName()+".COMMAND");
					} else {
						System.out.println("\nDroolsService->receiveCommand->Firing-->"+itemState.getClass());		
						factHashMap.put(item.getName()+".COMMAND", command.toString());
						//droolsScriptExecuter.executeRule(session, itemRegistry,factHashMap);
						kStatefulSession	=	droolsSessionDataHolder.getStatefulSession();	
						IDroolsDTO	dto	=	droolsSessionDataHolder.buildFacts(command,itemState);
						droolsSessionDataHolder.insertHashMap(factHashMap);
						droolsSessionDataHolder.insertObject(dto);
						droolsSessionDataHolder.executeStatefulRule();
						droolsSessionDataHolder.disposeSession();
						factHashMap.remove(item.getName()+".COMMAND");
						
					}
					
					
				} else {
					System.out.println("\nDroolsService->receiveCommand-Not Firing->"+itemState);
				}
				System.out.println("\nDroolsEngine->receiveCommand->3->item"+item+"->newState->"+command);
			} catch (ItemNotFoundException e) {
				// ignore commands for non-existent items
			}
		}
	}
	
	private boolean validateRuleExecution(String itemName,Command command){
		System.out.println("\nDroolsService->validateRuleExecution->itemName->"+itemName+"->command->"+command.toString());
		if(itemRulesMap.containsKey(itemName)){
			//This means there is a rule configured for this.
			List<String>	ruleTypeList	=	itemRulesMap.get(itemName);
			if(ruleTypeList.contains("COMMAND")){
				return true;
			}
		}
		
		return false;
	}

	public void fireScheduledRule(IDroolsDTO	dto){
			
		CommonDroolsDTO	commonDroolsDTO	=	(CommonDroolsDTO)dto;
		System.out.println("\nDroolsService->fireScheduledRule->Firing-->DONE=>---"+commonDroolsDTO.getIS_TIMETYPE());	
		kStatefulSession	=	droolsSessionDataHolder.getStatefulSession();
		factHashMap.put(commonDroolsDTO.getIS_TIMETYPE(), CloudAppConstants.YES);
		droolsSessionDataHolder.insertHashMap(factHashMap);
		droolsSessionDataHolder.insertObject(dto);
		droolsSessionDataHolder.executeStatefulRule();
		factHashMap.remove(commonDroolsDTO.getIS_TIMETYPE());
		droolsSessionDataHolder.disposeSession();
	}
	
}
