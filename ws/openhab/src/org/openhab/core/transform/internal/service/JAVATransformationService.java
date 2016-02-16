package org.openhab.core.transform.internal.service;

import org.openhab.core.items.Item;
import org.openhab.core.parser.InboundFixedLengthParser;
import org.openhab.core.parser.dto.InboundMessageDTO;
import org.openhab.core.transform.CloudTransformationHelper;
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
		System.out.println("\nJAVATransformationService->transform->returnValue->"+returnValue);
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

	@Override
	public InboundMessageDTO transformInboundMessage(String function, String source,
			String itemName) throws TransformationException {
		// TODO Auto-generated method stub
		String	status	=	null;
		InboundMessageDTO	inboundMessageDTO	=	null;
		
		if(function!=null && function.equals(CloudTransformationHelper.SWITCHTYPEITEM)){
			//SwitchItem
			inboundMessageDTO	=	(new InboundFixedLengthParser()).parseInboundSwitchItemMessage(source);
		} else if(function!=null && function.equals(CloudTransformationHelper.CONTACTTYPEITEM)){
			inboundMessageDTO	=	(new InboundFixedLengthParser()).parseInboundContactItemMessage(source);
		}
		System.out.println("\nJAVATransformationService->"+status);
		return inboundMessageDTO;
	}
	
	
	
}
