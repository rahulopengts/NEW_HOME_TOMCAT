package org.openhab.core.drools.internal;

import java.util.Arrays;
import java.util.Collection;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

import org.kie.api.runtime.StatelessKieSession;
import org.openhab.core.items.Item;
import org.openhab.core.items.ItemRegistry;
import org.openhab.core.types.State;

public class DroolsScriptExecuter {

	
	public void executeRule(StatelessKieSession session, ItemRegistry itemRegistry,Map<String,String> factHashMap){
		try{
			Collection<Item> collectionItem	=	itemRegistry.getItems();
			Iterator<Item> itemIterator	=	collectionItem.iterator();
	        session.execute(Arrays.asList(factHashMap));
		} catch (Throwable e){
			e.printStackTrace();
		}
	}
}
