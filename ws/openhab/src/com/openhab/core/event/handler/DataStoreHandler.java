package com.openhab.core.event.handler;

import java.util.HashMap;
import java.util.Map;

import org.openhab.core.items.ItemRegistry;
import org.openhab.core.persistence.internal.PersistenceManager;
import org.openhab.model.rule.internal.engine.RuleEngine;

import com.google.common.eventbus.AllowConcurrentEvents;
import com.google.common.eventbus.Subscribe;
import com.openhab.core.event.dto.EventObject;
import com.openhab.core.internal.event.dto.CloudEvent;
import com.openhab.core.internal.event.dto.CloudEventProperty;
import com.openhab.core.internal.event.processor.CloudAutoUpdateBinding;
import com.openhab.core.internal.event.processor.CloudEventPublisherImpl;
import com.openhab.core.internal.event.processor.ext.ExtAutoUpdateBinding;

public class DataStoreHandler extends AbstractEventHandler {
/*
 * 	This method is invoked when
 *  1. 	sendCommand has to be published. This command is supposed to Publish the Data as well as fire the related rules.
 *  2. 	MqttItemBinding : Calling handleEvent will check if the messageType is COMMAND and Binding also has COMMAND then transformation message is fired 
 *  	on the configured QUEUE
 *  3. 	handleEvent is also invoked on CloudAutoUpdateBinding. This iterated through all the listeners and Invokes GeneriItems. receiveCommand of CloudAutoUpdateBinding.
 *  	This calls postUpdate on GenericItem on the Item which has issued command. This internally calls all the StateChangeListener on the Item.
 *  4.  During this RuleEngine / PesistenceManager are invoked.
 *  5.  In nutshell this will lead to MqttItemConfig(Publish provided Command is instance of State), RulesEngine and PersistenceManager.
 *  6.  Message should be published to this only when MQTT Binding or Any other binding (as registered during webappservlet) is to be invoked to publish message
 *  	and Rules has to be invoked along with persisting the message.
 */
	
	int i	=	0;
	//@Override
	@Subscribe
	@AllowConcurrentEvents
	public void handleEvent(EventObject eventObject) {
		// TODO Auto-generated method stub
		i++;
		System.out.println("\n DataStoreHandler->handleEvent->itemName"+eventObject.getItemName()+"->command->"+eventObject.getCommand());
		ItemRegistry	itemRegistrty	=	eventObject.getItemRegistry();

		PersistenceManager	persistanceManager	=	eventObject.getPersistanceManager();
		RuleEngine	ruleEngine	=	eventObject.getRuleEngine();
		
		CloudEventPublisherImpl	cloudEventPublisherImpl	=	new CloudEventPublisherImpl();
		cloudEventPublisherImpl.setItemRegistry(itemRegistrty);
		cloudEventPublisherImpl.setModelRepository(eventObject.getModelRepository());
		cloudEventPublisherImpl.setPersistenceManager(persistanceManager);
		cloudEventPublisherImpl.setRuleEngine(ruleEngine);
		cloudEventPublisherImpl.setSiteName(eventObject.getSiteName());
		cloudEventPublisherImpl.sendCommand(eventObject.getItemName(), eventObject.getCommand());
		System.out.println("\n DataStoreHandler->handleEvent->1");
		
//		CloudAutoUpdateBinding	cloudAutoUpdateBinding	=	new CloudAutoUpdateBinding();
//		CloudEvent	cloudEvent	=	null;//new CloudEvent("",new Properties() );
//		
//		Map	property	=	new HashMap<String, Object>();
//		property.put("topic","");
//		property.put("state",eventObject.getNewState());
//		CloudEventProperty	cloudProperty	=	new CloudEventProperty(property);
//		CloudEvent	event	=	new CloudEvent(eventObject.getTopicName(), cloudProperty);
//		cloudAutoUpdateBinding.handleEvent(cloudEvent);

	}
	
}
