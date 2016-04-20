package com.openhab.core.util;

import java.io.ByteArrayOutputStream;
import java.io.InputStream;
import java.sql.Blob;

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
	
	public static String getStringfromBlob(Blob blob){
		String blobString = null;
		try{
			
			ByteArrayOutputStream baos = new ByteArrayOutputStream();
			byte[] buf = new byte[1024];
	
			InputStream in = blob.getBinaryStream();
	
			int n = 0;
			while ((n=in.read(buf))>=0)
			{
			   baos.write(buf, 0, n);
			}
	
			in.close();
			byte[] bytes = baos.toByteArray();
			blobString = new String(bytes);
		} catch (Exception e){
			e.printStackTrace();
		}
		
		return blobString;
		
	}
}
