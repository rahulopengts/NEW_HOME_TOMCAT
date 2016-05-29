package com.firebase.java;
import com.firebase.client.Firebase;
import com.firebase.client.FirebaseError;

import java.util.Date;
import java.util.concurrent.atomic.AtomicBoolean;

public class Demo {

    public static void main(String[] args) {
        //final AtomicBoolean done = new AtomicBoolean(false);
		boolean test	=	false;
        Firebase ref = new Firebase("https://project-8241275639547446807.firebaseio.com/");
        ref.setValue(args[0], new Firebase.CompletionListener() {
            @Override
            public void onComplete(FirebaseError firebaseError, Firebase firebase) {
                System.out.println("\n Done with Writing");
				//done.set(true);
				test	=	true;
            }
        });
        //while (!done.get()){
			while(!test){
			System.out.println("\n Checking ");
		} 
		System.out.println("\n All Done ");
    }
}