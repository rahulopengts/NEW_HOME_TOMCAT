package org.openhab.core.drools.dto;

public class AbstractDroolsDTO implements IDroolsDTO {

	private String itemName		=	null;
	
	private String commandName		=	null;
	
	private int value	=	0;
	

	//private int setItemStateInt
	
	public int getValue() {
		return value;
	}
	public void setValue(int value) {
		this.value = value;
	}
	public String getCommandName() {
		return commandName;
	}
	public void setCommandName(String commandName) {
		this.commandName = commandName;
	}
	public String getItemName() {
		return itemName;
	}
	public void setItemName(String itemName) {
		this.itemName = itemName;
	}
	public String getItemState() {
		return itemState;
	}
	public void setItemState(String itemState) {
		this.itemState = itemState;
	}
	public String getSiteName() {
		return siteName;
	}
	public void setSiteName(String siteName) {
		this.siteName = siteName;
	}
	private String itemState	=	null;
	private String siteName		=	null;
	
}
