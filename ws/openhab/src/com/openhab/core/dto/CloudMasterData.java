package com.openhab.core.dto;

import org.openhab.binding.mqtt.internal.MqttGenericBindingProvider;
import org.openhab.core.drools.internal.DroolsService;
import org.openhab.core.items.ItemRegistry;
import org.openhab.core.persistence.internal.PersistenceManager;
import org.openhab.core.types.Command;
import org.openhab.model.core.ModelRepository;
import org.openhab.model.rule.internal.engine.RuleEngine;
import org.openhab.model.sitemap.SitemapProvider;
import org.openhab.ui.items.ItemUIRegistry;
import org.openhab.ui.webapp.internal.render.PageRenderer;

import com.homeauto.core.mqtt.ICloudMqttMessagePubSub;
import com.openhab.core.event.dto.EventObject;

public class CloudMasterData implements ICloudMasterData{

	private MqttGenericBindingProvider	mqttGenericBindingProvider	=	null;
	
	public MqttGenericBindingProvider getMqttGenericBindingProvider() {
		return mqttGenericBindingProvider;
	}
	public void setMqttGenericBindingProvider(
			MqttGenericBindingProvider mqttGenericBindingProvider) {
		this.mqttGenericBindingProvider = mqttGenericBindingProvider;
	}
	private ICloudMqttMessagePubSub	subscriber	=	null;
	public ICloudMqttMessagePubSub getSubscriber() {
		return subscriber;
	}
	public void setSubscriber(ICloudMqttMessagePubSub subscriber) {
		this.subscriber = subscriber;
	}
	public ICloudMqttMessagePubSub getPublisher() {
		return publisher;
	}
	public void setPublisher(ICloudMqttMessagePubSub publisher) {
		this.publisher = publisher;
	}
	private ICloudMqttMessagePubSub	publisher	=	null;
	
	private SitemapProvider	siteMapProvider	=	null;
	private PageRenderer	pageRenderer	=	null;
	private ItemRegistry	itemRegistry	=	null;
	private ModelRepository	modelRepository	=	null;
	private DroolsService	droolsService	=	null;
	
	private ItemUIRegistry	itemUIRegistry	=	null;
	
	
	public ItemUIRegistry getItemUIRegistry() {
		return itemUIRegistry;
	}
	public void setItemUIRegistry(ItemUIRegistry itemUIRegistry) {
		this.itemUIRegistry = itemUIRegistry;
	}
	public DroolsService getDroolsService() {
		return droolsService;
	}
	public void setDroolsService(DroolsService droolsService) {
		this.droolsService = droolsService;
	}
	private String topicName		=	null;
	private Command command		=	null;
	private String itemName		=	null;
	private EventObject	eventObject	=	null;

	private PersistenceManager	persistenceManager	=	null;
	private RuleEngine	ruleEngine	=	null;
	
	
	public RuleEngine getRuleEngine() {
		return ruleEngine;
	}
	public void setRuleEngine(RuleEngine ruleEngine) {
		this.ruleEngine = ruleEngine;
	}
	public PersistenceManager getPersistenceManager() {
		return persistenceManager;
	}
	public void setPersistenceManager(PersistenceManager persistenceManager) {
		this.persistenceManager = persistenceManager;
	}
	public PageRenderer getPageRenderer() {
		return pageRenderer;
	}
	public void setPageRenderer(PageRenderer pageRenderer) {
		this.pageRenderer = pageRenderer;
	}
	public SitemapProvider getSiteMapProvider() {
		return siteMapProvider;
	}
	public void setSiteMapProvider(SitemapProvider siteMapProvider) {
		this.siteMapProvider = siteMapProvider;
	}

	
	public ItemRegistry getItemRegistry() {
		return itemRegistry;
	}
	public void setItemRegistry(ItemRegistry itemRegistry) {
		this.itemRegistry = itemRegistry;
	}
	public ModelRepository getModelRepository() {
		return modelRepository;
	}
	public void setModelRepository(ModelRepository modelRepository) {
		this.modelRepository = modelRepository;
	}
	public String getTopicName() {
		return topicName;
	}
	public void setTopicName(String topicName) {
		this.topicName = topicName;
	}
	public Command getCommand() {
		return command;
	}
	public void setCommand(Command command) {
		this.command = command;
	}
	public String getItemName() {
		return itemName;
	}
	public void setItemName(String itemName) {
		this.itemName = itemName;
	}
	public EventObject getEventObject() {
		return eventObject;
	}
	public void setEventObject(EventObject eventObject) {
		this.eventObject = eventObject;
	}
	
}
