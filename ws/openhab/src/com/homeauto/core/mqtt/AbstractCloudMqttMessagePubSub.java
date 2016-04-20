package com.homeauto.core.mqtt;

import java.util.HashMap;
import java.util.Map;

import org.openhab.io.transport.mqtt.MqttMessageConsumer;
import org.openhab.io.transport.mqtt.MqttMessageProducer;

public class AbstractCloudMqttMessagePubSub implements ICloudMqttMessagePubSub {

//	private List<MqttMessageConsumer> consumers = new ArrayList<MqttMessageConsumer>();
//	private List<MqttMessageProducer> producers = new ArrayList<MqttMessageProducer>();
	

	private HashMap<String,MqttMessageConsumer> consumers = new HashMap<String,MqttMessageConsumer>();
	public HashMap<String, MqttMessageConsumer> getConsumers() {
		return consumers;
	}

	public void setConsumers(HashMap<String, MqttMessageConsumer> consumers) {
		this.consumers = consumers;
	}

	public Map<String, MqttMessageProducer> getProducers() {
		return producers;
	}

	public void setProducers(HashMap<String, MqttMessageProducer> producers) {
		this.producers = producers;
	}

	private Map<String,MqttMessageProducer> producers = new HashMap<String,MqttMessageProducer>();
	@Override
	public void addConsumer(String key, MqttMessageConsumer subscriber) {
		// TODO Auto-generated method stub
		consumers.put(key, subscriber);
	}

	@Override
	public void addProducer(String key, MqttMessageProducer msgProducer) {
		// TODO Auto-generated method stub
		producers.put(key, msgProducer);
	}

	@Override
	public MqttMessageConsumer getConsumers(String key) {
		// TODO Auto-generated method stub
		return consumers.get(key);
	}

	@Override
	public MqttMessageProducer getProducers(String key) {
		// TODO Auto-generated method stub
		return producers.get(key);
	}
}
