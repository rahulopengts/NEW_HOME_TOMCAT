package org.openhab.io.rest.internal.resources;

import com.sun.jersey.api.json.JSONWithPadding;

public class ResponseHelper {
	
	public static Object wrapContentIfNeccessary(String callback,
			final String responseType, final Object content) {
		if (responseType.equals(MediaTypeHelper.APPLICATION_X_JAVASCRIPT)) {
			JSONWithPadding j	=	new JSONWithPadding(content, callback);
			System.out.println("\nwrapContentIfNeccessady->"+j.toString());
			return j;
		}
		return content;
	}
	
}
