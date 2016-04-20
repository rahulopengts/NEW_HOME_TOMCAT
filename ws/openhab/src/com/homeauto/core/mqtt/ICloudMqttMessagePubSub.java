package com.homeauto.core.mqtt;

import java.util.HashMap;

import org.openhab.binding.mqtt.internal.MqttMessageSubscriber;
import org.openhab.io.transport.mqtt.MqttMessageConsumer;
import org.openhab.io.transport.mqtt.MqttMessageProducer;

public interface ICloudMqttMessagePubSub {

	public void addConsumer(String key,MqttMessageConsumer subscriber);
	
	public void addProducer(String key,MqttMessageProducer producer);

	public MqttMessageConsumer getConsumers(String key);
	
	public MqttMessageProducer getProducers(String key);

}
