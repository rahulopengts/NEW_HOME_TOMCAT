package com.openhab.core.util;

import org.openhab.core.library.types.IncreaseDecreaseType;

public class UIComponentValueCalculator {

	
	public static String getNextValue(String commandType,int currentValue,int incrementValue,int maxValue){
		int nextVal	=	0;
		System.out.println("\nUIComponentValue->getNextValue->"+IncreaseDecreaseType.INCREASE.toString());
		if(commandType!=null && commandType.equals(IncreaseDecreaseType.INCREASE.toString())){
			nextVal	=	currentValue+incrementValue;
			System.out.println("\nUIComponentValue->getNextValue->"+nextVal);
		}
		
	return Integer.toString(nextVal);
	}
}
