package com.firebase.client.java.listener;

import com.firebase.client.Firebase;
import com.firebase.client.FirebaseError;

public class FirebaseEventListener implements Firebase.CompletionListener {

	boolean completionFlag	=	false;
	Object object	=	new Object();
	public boolean isCompletionFlag() {
		synchronized (object) {
			return completionFlag;
		}

	}

	public void setCompletionFlag(boolean completionFlag) {
		synchronized (object) {
			this.completionFlag = completionFlag;	
		}
		
	}

	@Override
	public void onComplete(FirebaseError arg0, Firebase arg1) {
		// TODO Auto-generated method stub
		setCompletionFlag(true);
		System.out.println("\n Done with Writing : "+isCompletionFlag());
	}

}
