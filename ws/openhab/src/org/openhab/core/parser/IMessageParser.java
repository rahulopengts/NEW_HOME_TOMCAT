package org.openhab.core.parser;

import org.openhab.core.parser.dto.InboundMessageDTO;

public interface IMessageParser {

	//~I 9 0 1 9 9 9 9 1  0   0   1   1   1  9  2  9  2  #
	//01 2 3 4 5 6 7 8 9  10  11  12  13  14 15 16 17 18 19
	public static int START_DELIMITER_INDEX	=	0;
	public static int END_DELIMITER_INDEX	=	19;
	public static int SWITCH_1_INDEX	=	9;
	public static int SWITCH_2_INDEX	=	10;
	public static int SWITCH_3_INDEX	=	11;
	public static int SWITCH_4_INDEX	=	12;
	public static int SWITCH_5_INDEX	=	13;
	public static int SWITCH_6_INDEX	=	14;
	
	
	//Contact Item Indexes
	public static int CONTACT_1_INDEX	=	9;
	
	
	public InboundMessageDTO parseInboundSwitchItemMessage(String value);
	
	public InboundMessageDTO parseInboundContactItemMessage(String value);
	
}
