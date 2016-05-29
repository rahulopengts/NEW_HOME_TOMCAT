package com.firebase.java;

import java.io.FileInputStream;
import java.util.HashMap;
import java.util.Map;

import com.google.firebase.FirebaseApp;
import com.google.firebase.FirebaseOptions;
import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.ValueEventListener;



public class MyFirebaseJava {
public static void main1(String str[]){
	try{
		FirebaseOptions options = new FirebaseOptions.Builder()
		  .setServiceAccount(new FileInputStream("D:/Home_Auto/firebase-google/myprojects/MyAwesomeProject-13d20a7ccfd4.json"))
		  .setDatabaseUrl("https://project-8241275639547446807.firebaseio.com/")
		  .build();
		FirebaseApp.initializeApp(options);
		
		// As an admin, the app has access to read and write all data, regardless of Security Rules
		DatabaseReference ref = FirebaseDatabase
		    .getInstance()
		    .getReference("restricted_access/secret_document");
		ref.addListenerForSingleValueEvent(new ValueEventListener() {
		    
		    public void onDataChange(DataSnapshot dataSnapshot) {
		        Object document = dataSnapshot.getValue();
		        System.out.println(document);
		    }

			public void onCancelled(DatabaseError arg0) {
				// TODO Auto-generated method stub
				
			}
		});
		
	} catch (Exception e){
		e.printStackTrace();
	}
}

public static void main(String str[]){
	try{
		// Initialize the app with a custom auth variable, limiting the server's access
		Map<String, Object> auth = new HashMap<String, Object>();
		auth.put("uid", "another_resource");
		
//	FirebaseOptions options = new FirebaseOptions.Builder()
//	  .setServiceAccount(new FileInputStream("D:/Home_Auto/firebase-google/myprojects/MyAwesomeProject-13d20a7ccfd4.json"))
//	  .setDatabaseUrl("https://project-8241275639547446807.firebaseio.com/")
//	  .build();
	
	FirebaseOptions options = new FirebaseOptions.Builder()
    .setDatabaseUrl("https://project-8241275639547446807.firebaseio.com/")
    .setServiceAccount(new FileInputStream("D:/Home_Auto/firebase-google/myprojects/MyAwesomeProject-13d20a7ccfd4.json"))
    .setDatabaseAuthVariableOverride(auth)
    .build();
	
	FirebaseApp.initializeApp(options);
	System.out.println("\n Firebase -1");
	// As an admin, the app has access to read and write all data, regardless of Security Rules
	DatabaseReference ref = FirebaseDatabase
	    .getInstance()
	    .getReference("User");
	
	System.out.println("\n Firebase -2");
	
	DatabaseReference usersRef = ref.child("User");

	System.out.println("\n Firebase -3");
	
	Map<String, User> users = new HashMap<String, User>();
	System.out.println("\n Firebase -4");
	
	users.put("User", new User("December 9, 1906", "Grace Hopper"));
	System.out.println("\n Firebase -5");
	User u	=	new User("June 23, 1912", "Alan Turing");
	System.out.println("\n Firebase -6");
	
	
	
	Firebase push = listRef.child("" + num);
	push.setValue(map);	
	
//	usersRef.setValue(u);
System.out.println("\n Firebase -11");
	
	} catch (Throwable e){
		e.printStackTrace();
	}
}

}
