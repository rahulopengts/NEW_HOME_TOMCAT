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

public class MyFirebaseJavaTest {
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
	FirebaseOptions options = new FirebaseOptions.Builder()
	  .setServiceAccount(new FileInputStream("D:/Home_Auto/firebase-google/myprojects/MyAwesomeProject-13d20a7ccfd4.json"))
	  .setDatabaseUrl("https://project-8241275639547446807.firebaseio.com/")
	  .build();
	FirebaseApp.initializeApp(options);
	
	// As an admin, the app has access to read and write all data, regardless of Security Rules
	DatabaseReference ref = FirebaseDatabase
	    .getInstance()
	    .getReference("myfirebasejavadata/one");
	
	DatabaseReference usersRef = ref.child("users");

//	Map<String, User> users = new HashMap<String, User>();
//	users.put("alanisawesome", new User("June 23, 1912", "Alan Turing"));
//	users.put("gracehop", new User("December 9, 1906", "Grace Hopper"));

	usersRef.setValue("myname");
	
	} catch (Throwable e){
		e.printStackTrace();
	}
}

}
