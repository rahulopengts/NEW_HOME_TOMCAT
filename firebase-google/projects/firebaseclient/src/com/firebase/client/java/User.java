package com.firebase.client.java;

import java.util.HashMap;
import java.util.Map;

public class User {

	private String fname	=	null;
	private String message	=	null;
	private Map<String, String> childMap	=	null;
	
	public Map<String, String> getChildMap() {
		return childMap;
	}
	public void setChildMap(Map<String, String> childMap) {
		this.childMap = childMap;
	}
	public User(String p_Fname,String p_Message){
		fname	=	p_Fname;
		message	=	p_Message;
	}
	
	public User(String p_Fname,String p_Message,Map child){
		fname	=	p_Fname;
		message	=	p_Message;
		childMap	=	child;
	}

	
	public String getFname() {
		return fname;
	}
	public void setFname(String fname) {
		this.fname = fname;
	}
	public String getMessage() {
		return message;
	}
	public void setMessage(String message) {
		this.message = message;
	}
}
