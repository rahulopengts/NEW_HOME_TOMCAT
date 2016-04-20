package com.openhab.core.util;
//http://www.a2ztechguide.com/2011/08/java-date-utility-methods.html

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.Locale;

public class DateUtil {

	public static final String DEFAULT_DATE_FORMAT	=	"dd-MM-yyyy";
	
	//dd/MM/yy
	//dd-MM-yy:HH:mm:SS
	

	public static boolean isDefaultFormatValidDate(String date) {
	    SimpleDateFormat sdf = new SimpleDateFormat("dd-MMM-yyyy", Locale.ENGLISH);
	    boolean isValid = true;
	    sdf.setLenient(false);
	    try {
	                sdf.parse(date);
	    } catch (Exception e) {
	                isValid = false;
	    }
	    // return true if date is valid otherwise return false
	    return isValid;
	}
	
	
	public static String convertDateFromOneFormatToOther(String fromDate){
        String convertedDate = "";
        try
        {
	        //create SimpleDateFormat object with source string date format
	        SimpleDateFormat sdfFrom = new SimpleDateFormat("dd/MM/yy");
	        sdfFrom.setLenient(false);
	        //parse the string into Date object
	        Date date = sdfFrom.parse(fromDate);
	        //create SimpleDateFormat object with desired date format
	        SimpleDateFormat sdfTo = new SimpleDateFormat("MM-dd-yyyy");
	        //parse the date into another format
	        convertedDate = sdfTo.format(date);
	        System.out.println("Date is converted from dd/MM/yy format to MM-dd-yyyy");
	        System.out.println("Converted date is : " + convertedDate);
        }
        catch(ParseException pe)
        {
                    return("Parse Exception : " + pe);
        }
        return convertedDate;
}
	
	public static void convertStringToDate(String dateString) {
        Date date = null;
        try
        {
	        //create SimpleDateFormat object with source string date format
	        SimpleDateFormat sdf = new SimpleDateFormat("dd-MMM-yyyy");
	        sdf.setLenient(false);
	        //parse the string into Date object
	        date = sdf.parse(dateString);
	        System.out.println("Date :"+date);
        }catch(ParseException pe){
                    System.out.println("Parse Exception : " + pe);
        }
        
     
}
	
    public static void getDayDetails() {
        // Get various information from the Date object.         
        Calendar cal = Calendar.getInstance();
        int day = cal.get(Calendar.DATE);
        int month = cal.get(Calendar.MONTH) + 1;
        int year = cal.get(Calendar.YEAR);
        
        System.out.println("Current Date: " + cal.getTime());
        System.out.println("Day: " + day);
        System.out.println("Month: " + month);
        System.out.println("Year: " + year);
    }    

    public static void convertToFormat(String[] args) {
        Calendar cal = Calendar.getInstance();
        /*
        set date format, this can be changed to whatever format
        you want, MM-dd-yyyy, MM.dd.yyyy, dd.MM.yyyy etc.
        you can read more about it here:
        http://java.sun.com/j2se/1.4.2/docs/api/index.html 
        */
      SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
      System.out.print(sdf.format(cal.getTime()));
}

}
