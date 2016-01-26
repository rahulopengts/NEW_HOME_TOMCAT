package org.openhab.core.drools.internal;

import static org.openhab.core.events.EventConstants.TOPIC_PREFIX;
import static org.openhab.core.events.EventConstants.TOPIC_SEPERATOR;
import static org.openhab.model.rule.internal.engine.RuleTriggerManager.TriggerTypes.COMMAND;

import java.io.File;
import java.util.Collection;
import java.util.Collections;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;


import org.apache.commons.io.filefilter.FileFilterUtils;
import org.apache.commons.io.filefilter.HiddenFileFilter;
import org.apache.commons.io.filefilter.IOFileFilter;
import org.apache.commons.io.monitor.FileAlterationMonitor;
import org.eclipse.xtext.naming.QualifiedName;
import org.kie.api.io.Resource;
import org.kie.api.runtime.StatelessKieSession;
import org.kie.api.runtime.rule.FactHandle;
import org.openhab.core.drools.util.DroolsUtil;
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
import org.openhab.model.rule.internal.engine.RuleContextHelper;
import org.openhab.model.rule.internal.engine.RuleEvaluationContext;
import org.openhab.model.rule.rules.Rule;
import org.osgi.service.event.Event;
import org.osgi.service.event.EventHandler;
import org.osgi.service.event.TopicPermission;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.common.collect.LinkedListMultimap;
import com.openhab.core.internal.event.dto.CloudEvent;

public class DroolsService  implements StateChangeListener , ItemRegistryChangeListener {//EventHandler,{

    private static final File RULES_DIR = new File("configurations/drools/");
    private static final IOFileFilter RULES_FILTER = FileFilterUtils.and(
            FileFilterUtils.fileFileFilter(),
            HiddenFileFilter.VISIBLE);
    private static final IOFileFilter RULES_DIR_FILTER = FileFilterUtils.and(
            FileFilterUtils.directoryFileFilter(),
            HiddenFileFilter.VISIBLE);

    private static final Logger logger = LoggerFactory.getLogger(DroolsService.class);

    private ItemRegistry itemRegistry;

    private Map<String, FactHandle> factHandles = new HashMap<String, FactHandle>();

    private final LinkedListMultimap<String, Object> updates = LinkedListMultimap.create();
    
    private DroolsScriptExecuter	droolsScriptExecuter	=	null;
    

    private StatelessKieSession session	=	null;

    private FileAlterationMonitor monitor;
    private Map<String, Resource> changedRules = Collections.synchronizedMap(new HashMap<String, Resource>());

    Map<String, String> factHashMap	=	new HashMap<>();
	public final static String FACT_OLD_SUFFIX	=	".OLD";
	public final static String FACT_NEW_SUFFIX	=	".NEW";

	
    public void activate() {
    	droolsScriptExecuter	=	new DroolsScriptExecuter();
    	
		String fileLoc	=	"D:\\Home_Auto\\NEW_HOME_TOMCAT\\ws\\DroolsTestProject\\conf\\hashmap.drl";
    	session = DroolsUtil.createStatelessKieSession(fileLoc);
        addItems(itemRegistry.getItems());
        logger.info("Dools service started");
    }
    


    private void addItems(Collection<Item> items) {
        for (Item item : items) {
            if (item instanceof GenericItem) {
                ((GenericItem) item).addStateChangeListener(this);
                System.out.println("\nDroolsService->addItems->"+item.getName());
        		State	itemState	=	item.getState();
        		factHashMap.put(item.getName()+FACT_NEW_SUFFIX, itemState.toString());
        		factHashMap.put(item.getName()+FACT_OLD_SUFFIX, itemState.toString());
            }
        }
    }

	@Override
	public void stateChanged(Item item, State oldState, State newState) {
		// TODO Auto-generated method stub
		System.out.println("\nDroolsService->stateChanged->"+item.getName()+"->oldState->"+oldState+"->newState->"+newState);		
		factHashMap.put(item.getName()+FACT_NEW_SUFFIX, newState.toString());
		factHashMap.put(item.getName()+FACT_OLD_SUFFIX, oldState.toString());		
		identifyFactChanges(item,oldState,newState);
		droolsScriptExecuter.executeRule(session, itemRegistry,factHashMap);
		factHashMap.put(item.getName()+FACT_OLD_SUFFIX, newState.toString());
		
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
				System.out.println("\nDroolsEngine->receiveCommand->2->itemName"+itemName+"->newState->"+command);
				Item item = itemRegistry.getItem(itemName);
//				Iterable<Rule> rules = triggerManager.getRules(COMMAND, item, command);
//				RuleEvaluationContext context = new RuleEvaluationContext();
//				context.newValue(QualifiedName.create(RuleContextHelper.VAR_RECEIVED_COMMAND), command);
//				executeRules(rules, context);
				System.out.println("\nDroolsEngine->receiveCommand->3->item"+item+"->newState->"+command);
			} catch (ItemNotFoundException e) {
				// ignore commands for non-existent items
			}
		}
	}
	
}
