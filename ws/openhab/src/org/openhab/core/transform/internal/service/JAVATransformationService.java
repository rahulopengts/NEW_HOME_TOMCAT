package org.openhab.core.transform.internal.service;

import org.openhab.core.items.Item;
import org.openhab.core.transform.TransformationException;
import org.openhab.core.transform.TransformationService;

public class JAVATransformationService implements TransformationService {

	@Override
	public String transform(String function, String source)
			throws TransformationException {
		// TODO Auto-generated method stub
		String returnValue	=	null;
		if(function!=null && source!=null){
			returnValue	=	function.replaceAll("%", source);
		}
		
		return returnValue;
	}

	@Override
	public String transform(String function, String source, Item item)
			throws TransformationException {
		// TODO Auto-generated method stub
		if(function!=null && source!=null){
			
		}
		return null;
	}
}
