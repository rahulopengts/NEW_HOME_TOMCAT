package com.automation.core.baas.messagin;

import com.firebase.client.Firebase;
import com.firebase.client.FirebaseError;
import com.firebase.client.java.listener.FirebaseEventListener;

public class DBaaSEventListener  implements Firebase.CompletionListener {


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
