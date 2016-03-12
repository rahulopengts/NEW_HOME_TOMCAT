package com.homeauto.db.core.dbprocessors;

public interface SiteQueryConstants {
	
	public static final String HSQLDB	=	"HSQLDB";
	
	public static final String DB_VENDOR	=	HSQLDB;	
	
	
	public static final String FETCH_SITE_INFO	=	"SELECT * FROM SITE_INFO WHERE USER_SITE_ID = ?";

	public static final String UPDATE_SITE_INFO_INITIALIZATION	=	"UPDATE SITE_INFO SET INITIALIZED = 1 WHERE SITE_ID =	?";

	public static final String INSERT_SITE_INFO	=	"INSERT INTO ITEM_INFO (SITE_ID,ITEM_ID,ITEM_VALUE,DATATYPE,LAST_UPDATE)  VALUES (?,?,?,?,?)";
	
	public static final String FETCH_ITEM_INFO	=	"SELECT * FROM ITEM_INFO WHERE SITE_ID = ?";

	public static final String UPDATE_ITEM_INFO	=	"UPDATE ITEM_INFO set ITEM_VALUE=? WHERE SITE_ID=? AND ITEM_ID=?";
}
