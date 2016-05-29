package com.firebase.java;

public class User {

	private String fname	=	null;
	private String message	=	null;

	public User(String p_Fname,String p_Message){
		fname	=	p_Fname;
		message	=	p_Message;
		
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
