package com.openhab.core.sysprop;

public class SystemPropUtil implements ISystemProp {
	
	public static String getSystemHome(){
		return APP_HOME; 
	}

	public static String getSystemHomeForSite(String strSiteId){
		
		return APP_HOME+strSiteId;
		 
	}
	
	public static String getSystemSiteMapPath(String strSiteId){
		return APP_HOME+strSiteId +"//sitemaps//";
	}
	
	public static String getSystemItemInfoPath(String strSiteId){
		return APP_HOME+strSiteId +"//items//";
	}

}
