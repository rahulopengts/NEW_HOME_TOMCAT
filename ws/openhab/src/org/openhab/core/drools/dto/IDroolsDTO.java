package org.openhab.core.drools.dto;

public interface IDroolsDTO {

	
	public int getValue();
	
	public void setValue(int value);
	
	public String getCommandName();
	
	public void setCommandName(String commandName);
	
	public String getItemName();
	
	public void setItemName(String itemName);
	
	public String getItemState();
	
	public void setItemState(String itemState);
	
	//public void setItemStateInt(int intStateValue);
	
	public String getSiteName();
	
	public void setSiteName(String siteName);
	

}
