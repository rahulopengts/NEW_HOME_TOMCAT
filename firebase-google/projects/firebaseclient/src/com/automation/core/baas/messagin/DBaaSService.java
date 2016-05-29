package com.automation.core.baas.messagin;

import java.util.HashMap;
import java.util.Map;

import com.firebase.client.Firebase;
import com.firebase.client.java.dto.Home;
import com.firebase.client.java.dto.device.Device;
import com.firebase.client.java.listener.FirebaseEventListener;
import com.shaded.fasterxml.jackson.databind.ObjectMapper;

public class DBaaSService {

	private Firebase fireBaseRootDBNode	=	null;
	
	private static DBaaSService	dbaaSService	=	null;
	private DBaaSEventListener	dbaaSEventListener	=	null;
	
	public static String DATABASE_PATH	=	"https://project-8241275639547446807.firebaseio.com/";
	
	public static DBaaSService getDBaaSService(){
		if(dbaaSService==null){
			dbaaSService	=	new DBaaSService();
			dbaaSService.initFirebase();
		}
		
		return dbaaSService;
	}
	
	private void initFirebase(){
		fireBaseRootDBNode	=	new Firebase(DATABASE_PATH);
		dbaaSEventListener	=	new DBaaSEventListener();
	}
	
	public Firebase getFireBaseRef(String firebaseDatabaseID){
		return fireBaseRootDBNode;
	}
	
	/*
	 * Adds a node the root of Database
	 */
	public void addFirstHomeAtDBNode(Map homeMap,String databasePath,String firebaseDatabaseID){
		Firebase ref 	=	getFireBaseRef(firebaseDatabaseID);
		dbaaSEventListener.setCompletionFlag(false);
		ref.setValue(homeMap,dbaaSEventListener);		
    	while(!dbaaSEventListener.isCompletionFlag()){
        }
    	System.out.println("\n Added First Home at the node ");    	
	}

	public void addDeviceNodeToHome(Device device,String pathToUpdate,String firebaseDatabaseID){
		Firebase ref 	=	getFireBaseRef(firebaseDatabaseID);
		dbaaSEventListener.setCompletionFlag(false);
	    //String updatePath	=	"HOME_2/deviceTypeMap/dev3";
		Firebase	childRef	=	ref.child(pathToUpdate);
		Map<String, Object> deviceDataMap = new ObjectMapper().convertValue(device, Map.class);
	    childRef.updateChildren(deviceDataMap,dbaaSEventListener);
	    while(!dbaaSEventListener.isCompletionFlag()){
	    }
	}

	public void addNewHomeAtDBRoot(Home myHomeAdd,String deviceId,String firebaseDatabaseID){
		Firebase ref 	=	getFireBaseRef(firebaseDatabaseID);
		dbaaSEventListener.setCompletionFlag(false);
		
		Firebase	childRefNewHome	=	ref.child(myHomeAdd.getHomeId());
        
    	Device dev4	=	(Device)myHomeAdd.getDeviceTypeMap().get(deviceId);
    	Map<String, Object> userMap2 = new ObjectMapper().convertValue(dev4, Map.class);
    	
    	Map newHomedevicesMap	=	new HashMap<String, Object>();
    	newHomedevicesMap.put("dev4",userMap2);
    	
    	myHomeAdd.setDeviceTypeMap(newHomedevicesMap);
    	Map<String, Object> newHomeuserMap2 = new ObjectMapper().convertValue(myHomeAdd, Map.class);
    	dbaaSEventListener.setCompletionFlag(false);
    	//Firebase newHomeRef	=	ref.child("");
    	childRefNewHome.updateChildren(newHomeuserMap2,dbaaSEventListener);
        while(!dbaaSEventListener.isCompletionFlag()){
        }
	}
	
	public void pushAPI(String value,String pathtoUpdate,String firebaseDatabaseID){
		
		Firebase ref 	=	getFireBaseRef(firebaseDatabaseID);
		dbaaSEventListener.setCompletionFlag(false);

		dbaaSEventListener.setCompletionFlag(false);
		//"HOME_2/deviceTypeMap/dev1/deviceStatus"
    	Firebase postRef = ref.child(pathtoUpdate);
		postRef.push().setValue(value,dbaaSEventListener);
		
    	while(!dbaaSEventListener.isCompletionFlag()){
        }
	}
	
	
}
