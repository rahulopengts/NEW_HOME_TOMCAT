package com.openhab.core.util;

import java.util.Iterator;

import org.apache.commons.configuration.CompositeConfiguration;
import org.apache.commons.configuration.PropertiesConfiguration;
import org.apache.commons.configuration.SystemConfiguration;
import org.openhab.ui.webapp.cloud.exception.CloudException;
import org.openhab.ui.webapp.cloud.exception.CloudExceptionManager;
import org.openhab.ui.webapp.cloud.exception.CloudMessageConstants;

public class AppPropertyReader {

	public static final String appConfigFileLoc	=	"D:/Home_Auto/NEW_HOME_TOMCAT/eclipse/appconfig";
	public static void init() throws Exception {
		 
        config = new CompositeConfiguration();
 
        // add config sources.
        // add SystemConfiguration first below we need to override properties
        // using java system properties
        config.addConfiguration(new SystemConfiguration());
        config.addConfiguration(new PropertiesConfiguration(appConfigFileLoc+"/config.properties"));
 
        System.out.println("----------------------------");
        System.out.println("Listing composite properties");
        System.out.println("----------------------------");
        Iterator<String> keys = config.getKeys();
        while (keys.hasNext()) {
            String key = keys.next();
            System.out.println(key + " = " + config.getProperty(key));
        }
 
    } 
	
	public static AppPropertyReader	appPropertyReader	=	null;
	
	public static CompositeConfiguration config = null;
	public static AppPropertyReader getAppPropertyReader() throws Exception{
		if(appPropertyReader==null){
			appPropertyReader	=	new AppPropertyReader();
			appPropertyReader.init();
		} else {
			
		}
		
		return appPropertyReader;
	}
	
	public static String getProperty(String key) throws CloudException,Exception {
		if(key==null){
			CloudExceptionManager.throwException(CloudMessageConstants.PROPERTY_KEY_NULL, new Exception(), null);
		}
		if(config==null){
			init();
		}
		
		return config.getString(key);
	}


public static void main(String ap[]){
	try{
	AppPropertyReader	a	=	new AppPropertyReader();
	String s	=	a.getAppPropertyReader().getProperty("test");
	System.out.println("\nKKK->"+s);
	} catch (CloudException e){
		e.printStackTrace();
	} catch (Exception e){
		e.printStackTrace();
	}
}
}
