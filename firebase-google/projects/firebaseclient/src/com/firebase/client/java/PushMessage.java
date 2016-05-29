package com.firebase.client.java;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import com.firebase.client.Firebase;
import com.firebase.client.java.dto.Home;
import com.firebase.client.java.dto.device.Device;
import com.firebase.client.java.dto.device.IDeviceType;
import com.firebase.client.java.listener.FirebaseEventListener;
import com.shaded.fasterxml.jackson.databind.ObjectMapper;

public class PushMessage {

	public Firebase ref = null;
	FirebaseEventListener	listen	=	null;
	
	public static String DATABASE_PATH	=	"https://project-8241275639547446807.firebaseio.com/";

	public static void main(String args[]){
		PushMessage pushMessage	=	new PushMessage();
		
		//-----------------Below Method works for most of the scenarios.-------------------//
pushMessage.test(args, DATABASE_PATH, null);
pushMessage.goOffiline();
		//-----------------Above method works----------------------------------------------//
		
//		
//		//Prepare Data for addFirstHomeAtDBNode API----
//    	Home myHome	=	new Home();
//    	Map<String, Object> devicesMap	=	new HashMap<String, Object>();
//    	myHome.setHomeId("HOME_2");
//    	Device dev1	=	new Device();
//    	dev1.setDeviceId("dev1");
//    	dev1.setDeviceLocation("Decor Room");
//    	dev1.setDeviceStatus("OFF");
//    	dev1.setDeviceType("SWITCH");
//    	devicesMap.put(dev1.getDeviceId(),dev1);
//    	
//    	Device dev2	=	new Device();
//    	dev2.setDeviceId("dev2");
//    	dev2.setDeviceLocation("Decor Room-2");
//    	dev2.setDeviceStatus("OFF");
//    	dev2.setDeviceType("SWITCH");
//    	devicesMap.put(dev2.getDeviceId(),dev2);
//    	
//    	myHome.setDeviceTypeMap(devicesMap);
//    	
//    	Map userMap	=	new HashMap<String, Home>();
//    	userMap.put(myHome.getHomeId(), myHome);
//    	
//    	//----------Calling API for First time add-----------
//    	pushMessage.addFirstHomeAtDBNode(userMap, DATABASE_PATH);
//		
//		//----------------------End--------------------------
//		
//		//---------Calling API to add a node within the Given node.
//		//Prepare Data for addNewHomeAtDBRoot API----
//		Map<String, Object> addDevicesMap	=	new HashMap<String, Object>();
//		
//    	Device dev3	=	new Device();
//    	dev2.setDeviceId("dev3");
//    	dev2.setDeviceLocation("Decor Room-3");
//    	dev2.setDeviceStatus("OFF");
//    	dev2.setDeviceType("SWITCH");
//    	
//        String updatePath	=	"HOME_2/deviceTypeMap/dev3";
//        pushMessage.addDeviceNodeToHome(dev3,updatePath);
//        
//        //Calling API to add new Root node
//        Home myHomeAdd	=	new Home();
//    	Map<String, Object> newHomedevicesMap	=	new HashMap<String, Object>();
//    	myHomeAdd.setHomeId("HOME_3");
//    	Device dev4	=	new Device();
//    	dev4.setDeviceId("dev4");
//    	dev4.setDeviceLocation("Decor Room-4");
//    	dev4.setDeviceStatus("OFF");
//    	dev4.setDeviceType("SWITCH");
//    	newHomedevicesMap.put(dev4.getDeviceId(), dev4);
//    	myHomeAdd.setDeviceTypeMap(newHomedevicesMap);
//		//---------Calling API to add a node within the Given node.    	
//    	pushMessage.addNewHomeAtDBRoot(myHomeAdd,"dev4");
//    	//---------End-----------------------------------
//    	
//    	pushMessage.goOffiline();
	}
	
	public void pushAPI(String value){
		if(ref == null){
        	ref = new Firebase(DATABASE_PATH);	
        } else {
        	System.out.println("\n Firebase already initialized");
        }
    	if(listen==null){
    		listen=new FirebaseEventListener();
    	}
    	listen.setCompletionFlag(false);
    	Firebase postRef = ref.child("HOME_2/deviceTypeMap/dev1/deviceStatus");
//		Map<String, String> post1 = new HashMap<String, String>();
//		post1.put("author", "gracehop");
//		post1.put("title", "Announcing COBOL, a New Programming Language");
		postRef.push().setValue(value,listen);
		
    	while(!listen.isCompletionFlag()){
        }

//		postRef.push().setValue("1");
//		postRef.push().setValue("2");
//		postRef.push().setValue("3");
//		Map<String, String> post2 = new HashMap<String, String>();
//		post2.put("author", "alanisawesome");
//		post2.put("title", "The Turing Machine");
//		postRef.push().setValue(post2);
	}
	/*
	 * Adds a node the root of Database
	 */
	public void addFirstHomeAtDBNode(Map homeMap,String databasePath){
		if(ref == null){
        	ref = new Firebase(databasePath);	
        } else {
        	System.out.println("\n Firebase already initialized");
        }
    	if(listen==null){
    		listen=new FirebaseEventListener();
    	}
        listen.setCompletionFlag(false);
    	ref.setValue(homeMap,listen);		
    	
    	while(!listen.isCompletionFlag()){
        }
		System.out.println("\n Added First Home at the node ");    	

	
	
	
	}

	public void addDeviceNodeToHome(Device device,String pathToUpdate){
		if(listen==null){
			listen=new FirebaseEventListener();
		}
	    listen.setCompletionFlag(false);
	    //String updatePath	=	"HOME_2/deviceTypeMap/dev3";
		Firebase	childRef	=	ref.child(pathToUpdate);
		Map<String, Object> deviceDataMap = new ObjectMapper().convertValue(device, Map.class);
	    childRef.updateChildren(deviceDataMap,listen);
	    while(!listen.isCompletionFlag()){
	    }
	}

	public void addNewHomeAtDBRoot(Home myHomeAdd,String deviceId){
        Firebase	childRefNewHome	=	ref.child(myHomeAdd.getHomeId());
    	Device dev4	=	(Device)myHomeAdd.getDeviceTypeMap().get(deviceId);
    	Map<String, Object> userMap2 = new ObjectMapper().convertValue(dev4, Map.class);
    	
    	Map newHomedevicesMap	=	new HashMap<String, Object>();
    	newHomedevicesMap.put("dev4",userMap2);
    	
    	myHomeAdd.setDeviceTypeMap(newHomedevicesMap);
    	Map<String, Object> newHomeuserMap2 = new ObjectMapper().convertValue(myHomeAdd, Map.class);
    	listen.setCompletionFlag(false);
    	//Firebase newHomeRef	=	ref.child("");
    	childRefNewHome.updateChildren(newHomeuserMap2,listen);
        while(!listen.isCompletionFlag()){
        }

	}
	
	public void test(String[] args,String databasePath,Map mapValue) {
        
		/*
		 * Code will add new Home as HOME_2 and three devices under this. 
		 * This is being used to add the very first Home. Here we are using
		 * setValue at root node.SetValue does not need conversion to the MAP object.
		 */
		if(ref == null){
        	ref = new Firebase(databasePath);	
        } else {
        	System.out.println("\n Firebase already initialized");
        }
    	if(listen==null){
    		listen=new FirebaseEventListener();
    	}
    	
    	Home myHome	=	new Home();
    	Map<String, Object> devicesMap	=	new HashMap<String, Object>();
    	
    	myHome.setHomeId("HOME_2");
    	
    	
    	Device dev1	=	new Device();
    	dev1.setDeviceId("dev1");
    	dev1.setDeviceLocation("Decor Room");
    	dev1.setDeviceStatus("OFF");
    	dev1.setDeviceType("SWITCH");
    	devicesMap.put(dev1.getDeviceId(),dev1);
    	
    	Device dev2	=	new Device();
    	dev2.setDeviceId("dev2");
    	dev2.setDeviceLocation("Decor Room-2");
    	dev2.setDeviceStatus("OFF");
    	dev2.setDeviceType("SWITCH2");
    	devicesMap.put(dev2.getDeviceId(),dev2);
    	
    	myHome.setDeviceTypeMap(devicesMap);
    	
    	Map userMap	=	new HashMap<String, Home>();
    	userMap.put(myHome.getHomeId(), myHome);
    	
        listen.setCompletionFlag(false);
    	ref.setValue(userMap,listen);
    	while(!listen.isCompletionFlag()){
        }
		System.out.println("\n Added Root Node HOME_2 ");

		/**
		 *  Below code adds a new device node. This adds a new node to the existing Home node. 
		 */
		
		Map<String, Object> addDevicesMap	=	new HashMap<String, Object>();
		
    	Device dev3	=	new Device();
    	dev2.setDeviceId("dev3");
    	dev2.setDeviceLocation("Decor Room-3");
    	dev2.setDeviceStatus("OFF");
    	dev2.setDeviceType("SWITCH");
    	
        String updatePath	=	"HOME_2/deviceTypeMap/dev3";
    	Firebase	childRef	=	ref.child(updatePath);
        
    	if(listen==null){
    		listen=new FirebaseEventListener();
    	}
        listen.setCompletionFlag(false);
    	
    	Map<String, Object> userMap1 = new ObjectMapper().convertValue(dev2, Map.class);
    	//addDevicesMap.put("dev3", userMap1);
    	
        childRef.updateChildren(userMap1,listen);
    	
        while(!listen.isCompletionFlag()){
        }
        System.out.println("\n Added Child Node to HOME_2 as dev3 ");

        //This code will add new Home
        /*
         * This code will add a new Home node at the root.
         */
        
        Firebase	childRefNewHome	=	ref.child("HOME_3");
    	Home myHomeAdd	=	new Home();
    	Map<String, Object> newHomedevicesMap	=	new HashMap<String, Object>();
    	myHomeAdd.setHomeId("HOME_3");
    	Device dev4	=	new Device();
    	dev4.setDeviceId("dev4");
    	dev4.setDeviceLocation("Decor Room-4");
    	dev4.setDeviceStatus("OFF");
    	dev4.setDeviceType("SWITCH");
    	Map<String, Object> userMap2 = new ObjectMapper().convertValue(dev4, Map.class);
    	newHomedevicesMap.put("dev4",userMap2);
    	myHomeAdd.setDeviceTypeMap(newHomedevicesMap);
    	Map<String, Object> newHomeuserMap2 = new ObjectMapper().convertValue(myHomeAdd, Map.class);
    	listen.setCompletionFlag(false);
    	//Firebase newHomeRef	=	ref.child("");
    	childRefNewHome.updateChildren(newHomeuserMap2,listen);
        while(!listen.isCompletionFlag()){
        }
        
        System.out.println("\n Added Root Node HOME_3 ");
        pushAPI("1");
        //pushAPI("OFF");
        pushAPI("2");
        pushAPI("3");
    }

	
	public void addNewHome(String[] args,String databasePath,String updatePath,Home user) {
        if(ref == null){
        	ref = new Firebase(databasePath);	
        } else {
        	System.out.println("\n Firebase already initialized");
        }
        
    	if(listen==null){
    		listen=new FirebaseEventListener();
    	}
    	
    	Map<String, Object> userMap = new ObjectMapper().convertValue(user, Map.class);
    	
    	Map updates	=	new HashMap<String, Object>();
    	updates.put(user.getHomeId(), userMap);
    	//updates.put("lists/"+uid+"/mine, "value");
    	listen.setCompletionFlag(false);
    	ref.updateChildren(updates,listen);
    	
        
    	
        while(!listen.isCompletionFlag()){
        }
        
		System.out.println("\n All Done ");
    }

	public static void mainOld(String args[]){
		PushMessage pushMessage	=	new PushMessage();
		String databaseNode	=	null;

		Map<String,String> childMap	=	new HashMap<String, String>();
		childMap.put("child1", "child1Value");
        Map<String, User> users = new HashMap<String, User>();
    	users.put("User1", new User("fname", "Rahul",childMap));
    	users.put("User2", new User("fanme", "Avishi"));
    	users.put("User3", new User("fanme", "Shweta"));
    	users.put("User4", new User("fanme", "Shweta"));

    	//Adding all new Nodes
    	pushMessage.saveKeyValueData(args,DATABASE_PATH,users);
		System.out.println("\n Saved-2! ");
		pushMessage.addNodeToPath(args,DATABASE_PATH,"User2/fname1","valueupdated");
        users = new HashMap<String, User>();
    	users.put("User5", new User("fanme", "Avishi5"));
    	pushMessage.addNodeChild(args,DATABASE_PATH,"/",new User("fanme", "Avishi5"));
		pushMessage.goOffiline();
	}
	
	public void initFirebase(){
		ref = new Firebase(DATABASE_PATH);
	}
	public void goOffiline(){
		ref.goOffline();
	}
    
	public void saveData(String[] args,String databasePath) {
        ref = new Firebase(databasePath);	
    	if(listen	== null){
    		listen	=	new FirebaseEventListener();
    	}
        
    	listen.setCompletionFlag(false);
        ref.setValue(args[0],listen);
        
        while(!listen.isCompletionFlag()){
        }
		System.out.println("\n All Done ");
    }
	
	//Saves at node where there is no initial data.
	public void saveKeyValueData(String[] args,String databasePath,Map mapValue) {
        if(ref == null){
        	ref = new Firebase(databasePath);	
        } else {
        	System.out.println("\n Firebase already initialized");
        }
    	if(listen==null){
    		listen=new FirebaseEventListener();
    	}
        listen.setCompletionFlag(false);
    	ref.setValue(mapValue,listen);
    	while(!listen.isCompletionFlag()){
        }
		System.out.println("\n All Done ");
    }

	public void addNodeToPath(String[] args,String databasePath,String updatePath,String updateValue) {
        if(ref == null){
        	ref = new Firebase(databasePath);	
        } else {
        	System.out.println("\n Firebase already initialized");
        }
        //"User2/fname"
        Firebase	childRef	=	ref.child(updatePath);
        
    	if(listen==null){
    		listen=new FirebaseEventListener();
    	}
        listen.setCompletionFlag(false);
    	childRef.setValue(updateValue,listen);
    	
        while(!listen.isCompletionFlag()){
        }
        
		System.out.println("\n All Done ");
    }
	
	//Refer-http://stackoverflow.com/questions/32848024/android-firebase-2-4-illegalstateexception-using-new-ref-updatechildren
	public void addNodeChild(String[] args,String databasePath,String updatePath,User user) {
        if(ref == null){
        	ref = new Firebase(databasePath);	
        } else {
        	System.out.println("\n Firebase already initialized");
        }
        
    	if(listen==null){
    		listen=new FirebaseEventListener();
    	}
    	
    	Map<String, Object> userMap = new ObjectMapper().convertValue(user, Map.class);
    	
    	Map updates	=	new HashMap<String, Object>();
    	updates.put("User5", userMap);
    	//updates.put("lists/"+uid+"/mine, "value");
    	listen.setCompletionFlag(false);
    	ref.updateChildren(updates,listen);
    	
        
    	
        while(!listen.isCompletionFlag()){
        }
        
		System.out.println("\n All Done ");
    }
	

	public void saveDataBoth(String[] args,String databasePath) {
        ref = new Firebase(databasePath);	
    	if(listen	== null){
    		listen	=	new FirebaseEventListener();
    	}
        listen.setCompletionFlag(false);
        ref.setValue(args[0],listen);
        
        while(!listen.isCompletionFlag()){
        }
        
        //--------------------------------------------------------------
    	Map<String, User> users = new HashMap<String, User>();
    	users.put("User", new User("Name", "Grace Hopper"));
    	
    	ref.setValue(users,listen);
        while(!listen.isCompletionFlag()){
        }
        
    	
        users = new HashMap<String, User>();
    	users.put("User2", new User("Name", "Shweta"));
    	ref.setValue(users,listen);
        while(!listen.isCompletionFlag()){
        }

        //--------------------------------------------------------------
        
		System.out.println("\n All Done ");
    }
	
}
