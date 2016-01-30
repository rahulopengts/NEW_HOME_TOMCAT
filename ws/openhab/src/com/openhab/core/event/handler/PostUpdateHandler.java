package com.openhab.core.event.handler;

import org.openhab.core.items.ItemRegistry;
import org.openhab.core.persistence.internal.PersistenceManager;
import org.openhab.core.types.State;
import org.openhab.model.rule.internal.engine.RuleEngine;

import com.google.common.eventbus.AllowConcurrentEvents;
import com.google.common.eventbus.Subscribe;
import com.openhab.core.event.dto.EventObject;
import com.openhab.core.internal.event.processor.CloudEventPublisherImpl;

public class PostUpdateHandler extends AbstractEventHandler {

	/*
	 * 	This is invoked on for status update and 
	 * 		1.This invokes ItemUpdater for the given Item. Only new states is passed on to this. Old state of Item is already in the existing object.
	 *  	2.The ItemUpdate calls setState on the GenericItem. This invokes the StateChangeListener on RulesEngine and PersistenceManager.
	 *  	3.So this is to be invoked only when Rule has to be fired and Data has to be persisted into DB.
	 */
	//@Override
		@Subscribe
		@AllowConcurrentEvents
		public void handleEvent(EventObject eventObject) {
			System.out.println("\n PostUpdateHandler->handleEvent->itemName"+eventObject.getItemName()+"->command->"+eventObject.getCommand());
			ItemRegistry	itemRegistrty	=	eventObject.getItemRegistry();

			PersistenceManager	persistanceManager	=	eventObject.getPersistanceManager();
			RuleEngine	ruleEngine	=	eventObject.getRuleEngine();
			
			CloudEventPublisherImpl	cloudEventPublisherImpl	=	new CloudEventPublisherImpl();
			cloudEventPublisherImpl.setItemRegistry(itemRegistrty);
			cloudEventPublisherImpl.setModelRepository(eventObject.getModelRepository());
			cloudEventPublisherImpl.setPersistenceManager(persistanceManager);
			cloudEventPublisherImpl.setRuleEngine(ruleEngine);
			cloudEventPublisherImpl.setSiteName(eventObject.getSiteName());
			cloudEventPublisherImpl.setCommand(eventObject.getCommand());
			cloudEventPublisherImpl.postUpdate(eventObject.getItemName(), (State)eventObject.getNewState());
			System.out.println("\n PostUpdateHandler->handleEvent->Done");
		}
}
