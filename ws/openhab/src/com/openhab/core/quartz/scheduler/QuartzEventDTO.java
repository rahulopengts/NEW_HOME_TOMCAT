package com.openhab.core.quartz.scheduler;

import java.io.Serializable;

public class QuartzEventDTO implements Serializable {

	private String name	=	null;

	public String getName() {
		return name;
	}

	public void setName(String name) {
		this.name = name;
	}

	
}
