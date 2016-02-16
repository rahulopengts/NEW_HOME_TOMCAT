package org.openhab.core.parser.dto;

public class InboundMessageDTO {

	private String messageType	=	null;
	private String itemStatusValue	=	null;
	
	public String getItemStatusValue() {
		return itemStatusValue;
	}
	public void setItemStatusValue(String itemStatusValue) {
		this.itemStatusValue = itemStatusValue;
	}
	public String getMessageType() {
		return messageType;
	}
	public void setMessageType(String messageType) {
		this.messageType = messageType;
	}
	public String getMessageFormat() {
		return messageFormat;
	}
	public void setMessageFormat(String messageFormat) {
		this.messageFormat = messageFormat;
	}
	public String getHomeId() {
		return homeId;
	}
	public void setHomeId(String homeId) {
		this.homeId = homeId;
	}
	public String getNodeId() {
		return nodeId;
	}
	public void setNodeId(String nodeId) {
		this.nodeId = nodeId;
	}
	private String messageFormat	=	null;
	private String homeId	=	null;
	private String nodeId	=	null;

}
