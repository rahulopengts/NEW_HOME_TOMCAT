package com.firebase.client.java.dto;

import java.util.List;
import java.util.Map;

import com.firebase.client.java.dto.device.IDeviceType;

public class Home {

	public String homeId	=	null;
	
	public String getHomeId() {
		return homeId;
	}

	public void setHomeId(String homeId) {
		this.homeId = homeId;
	}

	public Map<String,Object> deviceTypeMap	=	null;

	public Map<String, Object> getDeviceTypeMap() {
		return deviceTypeMap;
	}

	public void setDeviceTypeMap(Map<String, Object> deviceTypeMap) {
		this.deviceTypeMap = deviceTypeMap;
	}


}
