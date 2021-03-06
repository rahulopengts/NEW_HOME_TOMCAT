/**
 * Copyright (c) 2010-2015, openHAB.org and others.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 */
package org.openhab.binding.mqtt.internal;

import java.util.ArrayList;
import java.util.List;

import org.apache.commons.lang.StringUtils;
import org.openhab.core.events.EventPublisher;
import org.openhab.core.library.types.DateTimeType;
import org.openhab.core.library.types.DecimalType;
import org.openhab.core.library.types.HSBType;
import org.openhab.core.library.types.IncreaseDecreaseType;
import org.openhab.core.library.types.OnOffType;
import org.openhab.core.library.types.OpenClosedType;
import org.openhab.core.library.types.PercentType;
import org.openhab.core.library.types.StopMoveType;
import org.openhab.core.library.types.StringType;
import org.openhab.core.library.types.UpDownType;
import org.openhab.core.parser.dto.InboundMessageDTO;
import org.openhab.core.transform.CloudTransformationHelper;
import org.openhab.core.transform.TransformationService;
import org.openhab.core.types.Command;
import org.openhab.core.types.State;
import org.openhab.core.types.TypeParser;
import org.openhab.io.transport.mqtt.MqttMessageConsumer;
import org.openhab.model.item.binding.BindingConfigParseException;
import org.openhab.ui.webapp.cloud.exception.CloudException;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.openhab.core.constants.CloudAppConstants;
import com.openhab.core.event.handler.EventManager;
import com.openhab.core.ruleaction.DroolsBusEvent;
import com.openhab.core.util.AppPropertyReader;
import com.openhab.core.util.CloudHelperUtil;

/**
 * Message subscriber configuration for items which receive inbound MQTT
 * messages.
 * 
 * @author Davy Vanherbergen
 * @since 1.3.0
 */
public class MqttMessageSubscriber extends AbstractMqttMessagePubSub implements
		MqttMessageConsumer {

	private static Logger logger = LoggerFactory
			.getLogger(MqttMessageSubscriber.class);

	private EventPublisher eventPublisher;

	private String msgFilter = null;

	/**
	 * Create new MqttMessageSubscriber from config string.
	 * 
	 * @param configuration
	 *            config string
	 * @throws BindingConfigParseException
	 *             if the config string is invalid
	 */
	public MqttMessageSubscriber(String configuration)
			throws BindingConfigParseException {

		String[] config = splitConfigurationString(configuration);
		try {

			if (config.length != 4 && config.length != 5) {
				throw new BindingConfigParseException(
						"Configuration requires 4 or 5 parameters separated by ':'");
			}

			if (StringUtils.isEmpty(config[0])) {
				throw new BindingConfigParseException("Missing broker name.");
			} else {
				setBroker(config[0].trim());
			}

			if (StringUtils.isEmpty(config[1])) {
				throw new BindingConfigParseException("Invalid topic.");
			} else {
				setTopic(config[1].trim());
			}

			if (StringUtils.isEmpty(config[2])) {
				throw new BindingConfigParseException("Missing type.");
			} else {
				try {
					MessageType t = MessageType.valueOf(config[2].trim()
							.toUpperCase());
					setMessageType(t);
				} catch (IllegalArgumentException e) {
					throw new BindingConfigParseException("Invalid type.");
				}
			}

			if (StringUtils.isEmpty(config[3])) {
				throw new BindingConfigParseException(
						"Missing transformation configuration.");
			} else {
				setTransformationRule(config[3].trim());
				initTransformService();
			}
			if (config.length > 4) {
				setMsgFilter(config[4].trim());
			}

		} catch (BindingConfigParseException e) {
			throw new BindingConfigParseException("Configuration '"
					+ configuration
					+ "' is not a valid inbound configuration: "
					+ e.getMessage());
		}

	}

	@Override
	public void processMessage(String topic, byte[] message) {
		String homeId	=	null;
		System.out.println("\n MqttMessageSubscriber->processMessage->"+topic+":message:->"+new String(message)+"->this->"+this);
		InboundMessageDTO	inboundMessageDTO	=	null;
		try {
			System.out.println("\n MqttMessageSubscriber->processMessage->getTransformationServiceName():->"+getTransformationServiceName());
			
			if (getTransformationServiceName() != null
					&& getTransformationService() == null) {
				logger.debug("Received message before transformation service '{}' was initialized.");
				initTransformService();
			}

			String value = new String(message);
//			homeId	=	CloudHelperUtil.getHomeId(value);
//			System.out.println("\n MqttMessageSubscriber->processMessage->homeId->"+homeId);
//			
//			String userHomeId	=	AppPropertyReader.getProperty(CloudAppConstants.HOME_ID+"."+homeId);
			//************************************************//
			//Temp Code//
//			String tempMessage	=	value.substring(8,10);
//			//String tempMessage	=	value.substring(8,14);
//			
//			value	=	tempMessage;
			//************************************************//
			
		
			
			if (!msgFilterApplies(value)) {
				logger.debug(
						"Skipped message '{}' because Message Filter '{}' does not apply.",
						value, msgFilter);
				return;
			}

			if (getTransformationService() != null) {
//				value = getTransformationService().transform(
//						getTransformationServiceParam(), value);
				
				inboundMessageDTO = getTransformationService().transformInboundMessage(
						getTransformationServiceParam(), value,getItemName());
				value	=	inboundMessageDTO.getItemStatusValue();
				
				System.out.println("\n MqttMessageSubscriber->processMessage->getTransformationService().transform:->"+value);
			} else if (getTransformationRule() != null
					&& !getTransformationRule().equalsIgnoreCase("default")) {
				value = getTransformationRule();
				System.out.println("\n MqttMessageSubscriber->processMessage->getTransformationRule().transform:->"+value);
			}

			//homeId	=	CloudHelperUtil.getHomeId(value);//inboundMessageDTO.getHomeId();
			homeId	=	CloudHelperUtil.getHomeId(new String(message));//inboundMessageDTO.getHomeId();
			System.out.println("\n MqttMessageSubscriber->processMessage->homeId->"+homeId);
			
			String userHomeId	=	AppPropertyReader.getProperty(CloudAppConstants.HOME_ID+"."+homeId);
			System.out.println("\n MqttMessageSubscriber->processMessage->value():->"+value+"->messageFilter->"+msgFilter+"->homeid->"+userHomeId);				
			
			value = StringUtils.replace(value, "${itemName}", getItemName());
			System.out.println("\n MqttMessageSubscriber->processMessage->getTransformationService().transform:value repalce->"+value+"->:MessageType:->"+getMessageType());			
			if (getMessageType().equals(MessageType.COMMAND)) {
				//System.out.println("\n MqttMessageSubscriber->processMessage->getTransformationServiceName():->"+getTransformationServiceName());	
				Command command = getCommand(value);
				System.out.println("\n MqttMessageSubscriber->processMessage->getItemName"+getItemName()+":command:->"+command.toString());
				//eventPublisher.postCommand(getItemName(), command);
				
				DroolsBusEvent.sendCommand(getItemName(), command.toString(), userHomeId);
			} else {
//				IAppCache	cache	=	AppCacheFactory.getAppCacheInstance().getCacheImpl("");
//				CloudMasterData	master	=	cache.getFromCache("demo", null);
				
				State state = getState(value);
				EventManager manager	=	new EventManager();
				Command command = getCommand(value);
				//manager.postUpdate(getItemName(), command, "avishi");
				System.out.println("\n MqttMessageSubscriber->processMessage->getItemName"+getItemName()+"->State->"+state+":state:->"+state.toString());
				manager.postUpdate(getItemName(), state, userHomeId);
				//eventPublisher.postUpdate(getItemName(), state);
								
			}
		} catch (Exception e) {
			logger.error("Error processing MQTT message.", e);
		} catch (CloudException e){
			e.printStackTrace();
		}

	}

	/**
	 * Set the publisher to use for publishing openHAB updates.
	 * 
	 * @param eventPublisher
	 *            EventPublisher
	 */
	@Override
	public void setEventPublisher(EventPublisher eventPublisher) {
		this.eventPublisher = eventPublisher;
	}

	/**
	 * Set a Msg filter to the Subscriber. All Messages that do not match the
	 * filter will be ignored. The filter will be interpreted as regular
	 * expression. Set null to remove filter
	 * 
	 * @param filter
	 *            Regular Expression String
	 */
	public void setMsgFilter(String filter) {
		this.msgFilter = filter;
	}

	public String getMsgFilter() {
		return this.msgFilter;
	}

	/**
	 * Checks whether an incoming message matches a predefined regular
	 * expression filter
	 * 
	 * @param msg
	 * @return true if the msg matches the filter specified, or if no filter is
	 *         specified
	 */
	private boolean msgFilterApplies(String msg) {
		System.out.println("\n msgFileterApplies->"+msg+"->msgFilter->"+msgFilter);
		if (msg == null) {
			System.out.println("\n msgFileterApplies->msg==null");
			return false;
		} else if (msgFilter == null) {
			System.out.println("\n msgFileterApplies->msgFilter==null-true");
			return true;
		} else {
			System.out.println("\n msgFileterApplies->msg.matches(msgFilter)"+msg.matches(msgFilter));
			return msg.matches(msgFilter);
		}
	}

	/**
	 * Convert a string representation of a state to an openHAB State.
	 * 
	 * @param value
	 *            string representation of State
	 * @return State
	 */
	protected State getState(String value) {

		List<Class<? extends State>> stateList = new ArrayList<Class<? extends State>>();

		// Not sure if the sequence below is the best one..
		stateList.add(OnOffType.class);
		stateList.add(OpenClosedType.class);
		stateList.add(UpDownType.class);
		stateList.add(HSBType.class);
		stateList.add(PercentType.class);
		stateList.add(DecimalType.class);
		stateList.add(DateTimeType.class);
		stateList.add(StringType.class);

		return TypeParser.parseState(stateList, value);

	}

	/**
	 * Convert a string representation of a command to an openHAB command.
	 * 
	 * @param value
	 *            string representation of command
	 * @return Command
	 */
	protected Command getCommand(String value) {

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

	@Override
	public String getTopic() {
		return StringUtils.replace(super.getTopic(), "${item}", "+");
	}

	@Override
	protected void initTransformService() {
		// TODO Auto-generated method stub
		
		if (getTransformationService() != null || StringUtils.isBlank(getTransformationServiceName())) {
			return;
		}

		String transformationServiceType	=	getTransformationServiceName();
		System.out.println("\nMqttMessageSubscriber->initTransformService->"+getTransformationServiceName());
		
		
		if(transformationServiceType!=null && transformationServiceType.equals("JAVA")){
			TransformationService	transformationService	=	CloudTransformationHelper.getTransformationService(transformationServiceType);
			setTransformationService(transformationService);
			System.out.println("\nMqttMessageSubscriber->initTransformService->done for cloud");
			//TransformationService	transformationService	=	new 	
		} else {
			super.initTransformService();			
		}

	}
}
