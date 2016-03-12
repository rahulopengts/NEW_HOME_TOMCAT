package com.homeauto.db.core;

import java.sql.Connection;
import java.sql.SQLException;

import javax.naming.Context;
import javax.naming.InitialContext;
import javax.naming.NamingException;
import javax.sql.DataSource;

import org.openhab.ui.webapp.cloud.exception.CloudException;
import org.openhab.ui.webapp.cloud.exception.CloudExceptionManager;
import org.openhab.ui.webapp.cloud.exception.CloudMessageConstants;

public class DatabaseAccessManager {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub

	}
	
	private static Object lock	=	new Object();
	
	private static DatabaseAccessManager databaseAccessManager	=	null;
	
	private DataSource dataSource	=	null;
	
	private DatabaseAccessManager(){
		initialize();
	}
	
	public static DatabaseAccessManager getDatabaseAccessManager(){
		if(databaseAccessManager==null){
			synchronized(lock){
				databaseAccessManager	=	new DatabaseAccessManager();
				
			}
		}
		
		return databaseAccessManager;
	}
	

	private DataSource initialize(){
	        
			try {
	            // Get DataSource
	            Context initContext  = new InitialContext();
	            Context envContext  = (Context)initContext.lookup("java:/comp/env");
	            dataSource = (DataSource)envContext.lookup("jdbc/testdb");
	 
	             
	        } catch (NamingException e) {
	            e.printStackTrace();
	        }
	        return dataSource;
	    }

	
	public Connection getConnection() throws CloudException {
		Connection connection	=	null;
		try{
			connection	=	 dataSource.getConnection();
		} catch (SQLException e){
			e.printStackTrace();
			CloudExceptionManager.throwException(CloudMessageConstants.SQL_CONNECTION_ERROR, e, null);
		}
		
		return connection;
	}

}
