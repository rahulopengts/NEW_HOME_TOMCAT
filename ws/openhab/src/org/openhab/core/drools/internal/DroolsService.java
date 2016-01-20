package org.openhab.core.drools.internal;

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
import org.kie.api.io.Resource;
import org.kie.api.runtime.StatelessKieSession;
import org.kie.api.runtime.rule.FactHandle;
import org.openhab.core.drools.util.DroolsUtil;
import org.openhab.core.items.GenericItem;
import org.openhab.core.items.Item;
import org.openhab.core.items.ItemRegistry;
import org.openhab.core.items.ItemRegistryChangeListener;
import org.openhab.core.items.StateChangeListener;
import org.openhab.core.types.State;
import org.osgi.service.event.Event;
import org.osgi.service.event.EventHandler;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.common.collect.LinkedListMultimap;

public class DroolsService  implements EventHandler, ItemRegistryChangeListener, StateChangeListener {

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
            }
        }
    }

	@Override
	public void stateChanged(Item item, State oldState, State newState) {
		// TODO Auto-generated method stub
		System.out.println("\nDroolsService->stateChanged->"+item.getName()+"->oldState->"+oldState+"->newState->"+newState);		
		droolsScriptExecuter.executeRule(session, itemRegistry);
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

	@Override
	public void handleEvent(Event arg0) {
		// TODO Auto-generated method stub
		
	}

    public void setItemRegistry(ItemRegistry itemRegistry) {
        this.itemRegistry = itemRegistry;
        itemRegistry.addItemRegistryChangeListener(this);
    }

    public void unsetItemRegistry(ItemRegistry itemRegistry) {
        itemRegistry.removeItemRegistryChangeListener(this);
        this.itemRegistry = null;
    }

	
}
