package org.openhab.core.parser;

import org.openhab.core.parser.dto.InboundMessageDTO;

public class InboundFixedLengthParser extends AbstractMessageParser {
	//~I90199991001119292#
	//~I90199991001119292#
	public InboundMessageDTO parseInboundSwitchItemMessage(String source){
		String statusValue	=	null;
		InboundMessageDTO	inboundMessageDTO	=	null;
		System.out.println("\nInboundFixedLengthParser->parseInboundSwitchItemMessage->"+source);
		if(source!=null){
			statusValue	=	source.substring(SWITCH_1_INDEX,SWITCH_1_INDEX+1);
			inboundMessageDTO	=	new InboundMessageDTO();
			if(statusValue.equals("1")){
				statusValue	=	"ON";
				
			} else {
				statusValue	=	"OFF";
			}
			inboundMessageDTO.setItemStatusValue(statusValue);
		}
		return inboundMessageDTO;
	}

	public InboundMessageDTO parseInboundContactItemMessage(String source){
		String statusValue	=	null;
		InboundMessageDTO	inboundMessageDTO	=	null;
		System.out.println("\nInboundFixedLengthParser->parseInboundSwitchItemMessage->"+source);
		if(source!=null){
			statusValue	=	source.substring(CONTACT_1_INDEX,CONTACT_1_INDEX+1);
			inboundMessageDTO	=	new InboundMessageDTO();
			if(statusValue.equals("1")){
				statusValue	=	"OPEN";
			} else {
				statusValue	=	"CLOSED";
			}
			inboundMessageDTO.setItemStatusValue(statusValue);
		}
		return inboundMessageDTO;
	}
		
	
}
