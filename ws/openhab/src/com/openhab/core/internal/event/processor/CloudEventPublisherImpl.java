package com.openhab.core.internal.event.processor;

import static org.openhab.core.events.EventConstants.TOPIC_PREFIX;
import static org.openhab.core.events.EventConstants.TOPIC_SEPERATOR;

import java.util.Dictionary;
import java.util.Hashtable;

import org.openhab.binding.mqtt.internal.MqttGenericBindingProvider;
import org.openhab.binding.mqtt.internal.MqttItemBinding;
import org.openhab.core.drools.internal.DroolsService;
import org.openhab.core.events.EventPublisher;
import org.openhab.core.internal.items.ItemUpdater;
import org.openhab.core.items.ItemRegistry;
import org.openhab.core.persistence.internal.PersistenceManager;
import org.openhab.core.types.Command;
import org.openhab.core.types.EventType;
import org.openhab.core.types.State;
import org.openhab.model.core.ModelRepository;
import org.openhab.model.rule.internal.engine.RuleEngine;
import org.openhab.ui.webapp.cloud.exception.CloudException;
import org.osgi.service.event.EventAdmin;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.openhab.core.cache.AppCacheFactory;
import com.openhab.core.cache.IAppCache;
import com.openhab.core.dto.CloudMasterData;
import com.openhab.core.internal.event.dto.CloudEvent;
import com.openhab.core.threadstore.CloudThreadLocalStorage;

public class CloudEventPublisherImpl implements EventPublisher {

	private static final Logger logger = 
			LoggerFactory.getLogger(CloudEventPublisherImpl.class);
			
		private EventAdmin eventAdmin;
		private Command command	=	null;
		
		public Command getCommand() {
			return command;
		}

		public void setCommand(Command command) {
			this.command = command;
		}

		public String getSiteName() {
			return siteName;
		}

		public void setSiteName(String siteName) {
			this.siteName = siteName;
		}

		private String siteName	=	null;
		
		
		private ItemRegistry itemRegistry;
		
		private ModelRepository modelRepository;
		
		private PersistenceManager	persistenceManager	=	null;
		
		public PersistenceManager getPersistenceManager() {
			return persistenceManager;
		}

		public void setPersistenceManager(PersistenceManager persistenceManager) {
			this.persistenceManager = persistenceManager;
		}

		public RuleEngine getRuleEngine() {
			return ruleEngine;
		}

		public void setRuleEngine(RuleEngine ruleEngine) {
			this.ruleEngine = ruleEngine;
		}

		private RuleEngine	ruleEngine	=	null;
		
		
		public ModelRepository getModelRepository() {
			return modelRepository;
		}

		public void setModelRepository(ModelRepository modelRepository) {
			this.modelRepository = modelRepository;
		}

		public ItemRegistry getItemRegistry() {
			return itemRegistry;
		}

		public void setItemRegistry(ItemRegistry itemRegistry) {
			this.itemRegistry = itemRegistry;
		}

		public void setEventAdmin(EventAdmin eventAdmin) {
			this.eventAdmin = eventAdmin;
			System.out.println("\n EventPublisheImpl : setEventAdmin ->: "+eventAdmin.getClass().getName());
//			String s	=	null;
//			s.toString();
		}

		public void unsetEventAdmin(EventAdmin eventAdmin) {
			this.eventAdmin = null;
		}
		

		/* (non-Javadoc)
		 * @see org.openhab.core.internal.events.EventPublisher#sendCommand(org.openhab.core.items.GenericItem, org.openhab.core.datatypes.DataType)
		 */
		public void sendCommand(String itemName, Command command) {
			
			System.out.println("\n CloudEventPublisheImpl->sendCommand->");
			try{
			if (command != null) {
				
				ThreadLocal<String> t	=	new ThreadLocal<>();
				t.set("rahul");
				
/*				RuleEngine ruleEngine	=	new RuleEngine();
				System.out.println("\n CloudEventPublisheImpl->Initialize RuleEngine->2");
				ruleEngine.setItemRegistry(itemRegistry);
				System.out.println("\n CloudEventPublisheImpl->Initialize RuleEngine->3");
				ScriptEngine	scriptEngine	=	new ScriptEngineImpl();
				((ScriptEngineImpl)scriptEngine).activate();
				
				System.out.println("\n CloudEventPublisheImpl->Initialize RuleEngine->4");
				ruleEngine.setScriptEngine(scriptEngine);
				System.out.println("\n CloudEventPublisheImpl->Initialize RuleEngine->5");
				ruleEngine.setModelRepository(modelRepository);
				System.out.println("\n CloudEventPublisheImpl->Initialize RuleEngine->6");
				ruleEngine.activate();
*/				
				System.out.println("\n CloudEventPublisheImpl->sendCommand->Command is not null");
				System.out.println("\nCloudEventPublisheImpl->publishData->MasterData->"+Thread.currentThread().getId()+":MasterData:"+CloudThreadLocalStorage.getCloudMasterData());				
				
//				CloudMasterData	masterData	=	new CloudMasterData();
//				masterData.setItemRegistry(itemRegistry);
//				masterData.setModelRepository(modelRepository);
				
				//masterData.setTopicName(topicName);
				CloudEvent	cloudEvent	=	createCommandEvent(itemName, command);
//				CloudThreadLocalStorage.setCloudMasterData(masterData);
				
				CloudAutoUpdateBinding	cloudAutoUpdateBinding	=	new CloudAutoUpdateBinding();
				cloudAutoUpdateBinding.setItemRegistry(itemRegistry);
				cloudAutoUpdateBinding.handleEvent(cloudEvent);
				
				
				MqttGenericBindingProvider	mqttGenericBindingProvider	=	CloudMessageProcHelper.getMqttGenericBindingProvider(itemName, command,itemRegistry,modelRepository);
				MqttItemBinding	cloudMqttItemBinding	=	new MqttItemBinding();
				cloudMqttItemBinding.addBindingProvider(mqttGenericBindingProvider);
				cloudMqttItemBinding.receiveCommand(itemName, command);

			
				IAppCache	cache	=	AppCacheFactory.getAppCacheInstance(siteName);
				CloudMasterData masterData	=	(CloudMasterData)cache.getFromCache(siteName,null);
				DroolsService	droolsService	=	masterData.getDroolsService();
				droolsService.handleEvent(cloudEvent);
				
				
/*				ItemUpdater	itemUpdater	=	new ItemUpdater();
				itemUpdater.setItemRegistry(itemRegistry);
				itemUpdater.receiveCommand(itemName, command);
*/				

/*				System.out.println("\n CloudEventPublisheImpl->RuleEngine->1");
				

				System.out.println("\n CloudEventPublisheImpl->RuleEngine->7");
				ruleEngine.receiveCommand(itemName, command);
				System.out.println("\n CloudEventPublisheImpl->RuleEngine->8");
*/				
			} else {
				logger.warn("given command is NULL, couldn't send command to '{}'", itemName);
			}
			} catch (CloudException e){
				e.printStackTrace();
			}
		}

		/* (non-Javadoc)
		 * @see org.openhab.core.internal.events.EventPublisher#postCommand(org.openhab.core.items.GenericItem, org.openhab.core.datatypes.DataType)
		 */
		public void postCommand(String itemName, Command command) {
			if (command != null) {
				if(eventAdmin!=null){
					//eventAdmin.postEvent(createCommandEvent(itemName, command));
				}
					
			} else {
				logger.warn("given command is NULL, couldn't post command to '{}'", itemName);
			}
		}

		/* (non-Javadoc)
		 * @see org.openhab.core.internal.events.EventPublisher#postUpdate(org.openhab.core.items.GenericItem, org.openhab.core.datatypes.DataType)
		 */
		public void postUpdate(String itemName, State newState) {
			System.out.println("\n CloudEventPublisheImpl->sendCommand->");
			try{
				
				ThreadLocal<String> t	=	new ThreadLocal<>();
				t.set("rahul");
				System.out.println("\n CloudEventPublisheImpl->postUpdate->Command is not null");
				//System.out.println("\nCloudEventPublisheImpl->postUpdate->MasterData->"+Thread.currentThread().getId()+":MasterData:"+CloudThreadLocalStorage.getCloudMasterData());				
				CloudEvent	cloudEvent	=	createUpdateEvent(itemName, newState);
				
				
				
				MqttGenericBindingProvider	mqttGenericBindingProvider	=	CloudMessageProcHelper.getMqttGenericBindingProvider(itemName, command,itemRegistry,modelRepository);
				MqttItemBinding	cloudMqttItemBinding	=	new MqttItemBinding();
				System.out.println("\n CloudEventPublisheImpl->postUpdate->mqttGenericBindingProvider->"+mqttGenericBindingProvider.getItemConfig(itemName));
				cloudMqttItemBinding.addBindingProvider(mqttGenericBindingProvider);
				
				cloudMqttItemBinding.receiveUpdate(itemName, newState);
/*
				CloudAutoUpdateBinding	cloudAutoUpdateBinding	=	new CloudAutoUpdateBinding();
				cloudAutoUpdateBinding.setItemRegistry(itemRegistry);
				cloudAutoUpdateBinding.handleEvent(cloudEvent);

			
				IAppCache	cache	=	AppCacheFactory.getAppCacheInstance(siteName);
				CloudMasterData masterData	=	(CloudMasterData)cache.getFromCache(siteName,null);
				DroolsService	droolsService	=	masterData.getDroolsService();
				droolsService.handleEvent(cloudEvent);
				*/
				
				ItemUpdater	itemUpdater	=	new ItemUpdater();
				itemUpdater.setItemRegistry(itemRegistry);
				itemUpdater.receiveUpdate(itemName, newState);
				

			} catch (Exception e){
				e.printStackTrace();
			}
		}
		
		private CloudEvent createUpdateEvent(String itemName, State newState) {
			Dictionary<String, Object> properties = new Hashtable<String, Object>();
			properties.put("item", itemName);
			properties.put("state", newState);
			return new CloudEvent(createTopic(EventType.UPDATE, itemName) , properties);
		}

		private CloudEvent createCommandEvent(String itemName, Command command) {
			Dictionary<String, Object> properties = new Hashtable<String, Object>();
			properties.put("item", itemName);
			properties.put("command", command);
			System.out.println("\n CreateCommand : itemName "+itemName+" : Command "+command+" Command Object "+command.toString());
			return new CloudEvent(createTopic(EventType.COMMAND, itemName) , properties);
		}

		private String createTopic(EventType type, String itemName) {
			//System.out.println("\n CreateCommand : Topic "+TOPIC_PREFIX + TOPIC_SEPERATOR + type + TOPIC_SEPERATOR + itemName);
			return TOPIC_PREFIX + TOPIC_SEPERATOR + type + TOPIC_SEPERATOR + itemName;
		}
		
		

}
