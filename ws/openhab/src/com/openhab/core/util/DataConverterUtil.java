package com.openhab.core.util;

public class DataConverterUtil {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub

	}

	public static int covertToInt(String strValue, int defaultValue){
		int retValue	=	defaultValue;
		if(strValue!=null){
			try{
				retValue	=	Integer.parseInt(strValue);
			} catch (Exception e){
				
			}
		} 
		
		return retValue;
	}
}
