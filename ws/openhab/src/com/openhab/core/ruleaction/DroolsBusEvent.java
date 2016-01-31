package com.openhab.core.ruleaction;

import java.util.ArrayList;
import java.util.List;

import org.openhab.core.items.ItemRegistry;
import org.openhab.core.library.types.DecimalType;
import org.openhab.core.library.types.HSBType;
import org.openhab.core.library.types.IncreaseDecreaseType;
import org.openhab.core.library.types.OnOffType;
import org.openhab.core.library.types.OpenClosedType;
import org.openhab.core.library.types.PercentType;
import org.openhab.core.library.types.StopMoveType;
import org.openhab.core.library.types.StringType;
import org.openhab.core.library.types.UpDownType;
import org.openhab.core.persistence.internal.PersistenceManager;
import org.openhab.core.types.Command;
import org.openhab.core.types.TypeParser;
import org.openhab.model.core.ModelRepository;
import org.openhab.model.rule.internal.engine.RuleEngine;
import org.openhab.model.script.actions.BusEvent;
import org.openhab.ui.webapp.cloud.exception.CloudException;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.openhab.core.cache.AppCacheFactory;
import com.openhab.core.cache.IAppCache;
import com.openhab.core.dto.CloudMasterData;
import com.openhab.core.event.dto.EventObject;
import com.openhab.core.event.handler.EventManager;
import com.openhab.core.threadstore.CloudThreadLocalStorage;
import com.openhab.core.util.UIComponentValueCalculator;

public class DroolsBusEvent extends BusEvent {

	static private final Logger logger = LoggerFactory.getLogger(DroolsBusEvent.class);

	
	public static void sendCommand(String itemName, String commandString,String homeId) {
		
		System.out.println("\nDroolsBusEvent->sendCoammand->MasterData->"+Thread.currentThread().getId()+":MasterData:"+CloudThreadLocalStorage.getCloudMasterData());
		EventManager	eventManager	=	new EventManager();
		
			try {
				
				//EventManager manager	=	new EventManager();
				Command command = getCommand(commandString);
				//eventManager.postUpdate(itemName, command, "demo");
				IAppCache	cache	=	AppCacheFactory.getAppCacheInstance(homeId);
				CloudMasterData masterData	=	(CloudMasterData)cache.getFromCache(homeId,null);
				ItemRegistry	itemRegistry	=	masterData.getItemRegistry();
				ModelRepository	cloudModelRepository	=	masterData.getModelRepository();
				PersistenceManager pesPersistenceManager	=	masterData.getPersistenceManager();
				RuleEngine	ruleEngine	=	masterData.getRuleEngine();
				//EventObject	eventObject	=	masterData.getEventObject();
				EventObject	eventObject	=	new EventObject();
				
				eventObject.setCommand(command);
				eventObject.setModelRepository(cloudModelRepository);
				eventObject.setItemName(itemName);
				eventObject.setSiteName(homeId);
				eventObject.setPersistanceManager(pesPersistenceManager);
				eventObject.setRuleEngine(ruleEngine);
				eventObject.setItemRegistry(itemRegistry);
				eventManager.publishData(itemName, command, itemRegistry, masterData.getModelRepository(), masterData.getPersistenceManager(), masterData.getRuleEngine(), eventObject);
				//publishData(itemName, command, cloudItemRegistry,cloudModelRepository,persistenceManager,ruleEngine,eventObject);
				System.out.println("\nDroolsBusEvent->Done");
				
			} catch (CloudException e) {
				logger.warn("Item '" + itemName + "' does not exist.");
			}
		
	}

	/**
	 * Convert a string representation of a command to an openHAB command.
	 * 
	 * @param value
	 *            string representation of command
	 * @return Command
	 */
	protected static Command getCommand(String value) {

		List<Class<? extends Command>> commandList = new ArrayList<Class<? extends Command>>();

		commandList.add(OnOffType.class);
		commandList.add(OpenClosedType.class);
		commandList.add(UpDownType.class);
		commandList.add(IncreaseDecreaseType.class);
		commandList.add(StopMoveType.class);
		commandList.add(HSBType.class);
		commandList.add(PercentType.class);
		commandList.add(DecimalType.class);
		commandList.add(StringType.class);

		return TypeParser.parseCommand(commandList, value);

	}
	
	public static void postUpdate(String itemName, String commandString,String homeId) {
		EventManager	eventManager	=	new EventManager();
		//eventManager.postUpdate(itemName, command, siteName)
	}

	//public static void sendDimmerCommand(String itemName, String commandStringInceaseDecrease,int currentValue,int incrementFactor,int maxValue,String homeId) {

	public static void postDimmerCommand(String itemName, String commandStringInceaseDecrease,int increament,int maxValue,String homeId,int currentValue) {
		EventManager	eventManager	=	new EventManager();
		System.out.println("\nDroolsBusEvent->sendDimmerCommand->"+commandStringInceaseDecrease+"->"+increament+"->maxValue->"+maxValue+"->CurrentValue->"+currentValue);
		String nextValue	=	UIComponentValueCalculator.getNextValue(commandStringInceaseDecrease,currentValue, increament, maxValue);
		//System.out.println("\nDroolsBusEvent->sendDimmerCommand->"+homeId+"->commandName->"+commandStringInceaseDecrease+"->currentValue->"+currentValue+"->increamentFactor->"+incrementFactor+"->maxValue->"+maxValue);
		try {
			
			//EventManager manager	=	new EventManager();
			Command command = getCommand(nextValue);
			System.out.println("\nDroolsBusEvent->sendDimmerCommand->"+command.getClass());
			//eventManager.postUpdate(itemName, command, "demo");
			IAppCache	cache	=	AppCacheFactory.getAppCacheInstance(homeId);
			CloudMasterData masterData	=	(CloudMasterData)cache.getFromCache(homeId,null);
			ItemRegistry	itemRegistry	=	masterData.getItemRegistry();
			ModelRepository	cloudModelRepository	=	masterData.getModelRepository();
			PersistenceManager pesPersistenceManager	=	masterData.getPersistenceManager();
			RuleEngine	ruleEngine	=	masterData.getRuleEngine();
			//EventObject	eventObject	=	masterData.getEventObject();
			EventObject	eventObject	=	new EventObject();
			
			eventObject.setCommand(command);
			eventObject.setModelRepository(cloudModelRepository);
			eventObject.setItemName(itemName);
			eventObject.setSiteName(homeId);
			eventObject.setPersistanceManager(pesPersistenceManager);
			eventObject.setRuleEngine(ruleEngine);
			eventObject.setItemRegistry(itemRegistry);
			//eventManager.publishData(itemName, command, itemRegistry, masterData.getModelRepository(), masterData.getPersistenceManager(), masterData.getRuleEngine(), eventObject);
			eventManager.postUpdate(itemName, command, homeId);
			//publishData(itemName, command, cloudItemRegistry,cloudModelRepository,persistenceManager,ruleEngine,eventObject);
			System.out.println("\nDroolsBusEvent->Done");
		} catch (CloudException e){
			e.printStackTrace();
		}
		//eventManager.postUpdate(itemName, command, siteName)
	}
	
}
