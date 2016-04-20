package com.homeauto.db.core;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;

import org.openhab.ui.webapp.cloud.exception.CloudException;
import org.openhab.ui.webapp.cloud.exception.CloudExceptionManager;
import org.openhab.ui.webapp.cloud.exception.CloudMessageConstants;

public class AbstractDatabaseDAO implements IDataBaseDAO{

	private Connection connection	=	null;
	public boolean isConnectionOpen	=	false; 
	
	public Connection getConnection() {
		return connection;
	}

	public void setConnection(Connection connection) {
		this.connection = connection;
	}

	@Override
	public Connection getNewConnection(String dbType) throws CloudException{
		// TODO Auto-generated method stub
		try{
			System.out.println("\n HSQLDB getNewConnection->1");
			connection	=	DatabaseAccessManager.getDatabaseAccessManager().getConnection();
			System.out.println("\n HSQLDB getNewConnection->2");
		} catch (CloudException e){
			e.printStackTrace();
		} catch (Exception e){
			e.printStackTrace();
		} catch (Throwable e){
			e.printStackTrace();
		}
		isConnectionOpen	=	true;
		return connection;
		
	}

	@Override
	public void closeDbObjects(Statement statement, ResultSet resultSet,
			Connection connection)  throws CloudException {
		// TODO Auto-generated method stub
        try { 
        	if(null!=resultSet)resultSet.close();} 
        catch (SQLException e) {
        	e.printStackTrace();
        }
        try { 
        	if(null!=statement)statement.close();
        } catch (SQLException e) {
        	e.printStackTrace();
        }
        
        try { 
        	if(null!=connection){
        		connection.close();
        		isConnectionOpen	=	false;
        	}
        } catch (SQLException e) {
        	e.printStackTrace();
        }
		
	}

	@Override
	public void commit(Connection connection)  throws CloudException {
		try{
		if(connection!=null){
			if(!connection.isClosed()){
				connection.commit();
			}
		}
		} catch (SQLException e){
			e.printStackTrace();
			CloudExceptionManager.throwException(CloudMessageConstants.SQL_CONNECTION_ERROR, e, "");
		}
		// TODO Auto-generated method stub
		
	}

	@Override
	public void closeStmtAndResultset(Statement statement, ResultSet resultSet)
			throws CloudException {
		// TODO Auto-generated method stub
        try { 
        	if(null!=resultSet)resultSet.close();} 
        catch (SQLException e) {
        	e.printStackTrace();
        }
        try { 
        	if(null!=statement)statement.close();
        } catch (SQLException e) {
        	e.printStackTrace();
        }
		
	}

}
