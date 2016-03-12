package com.homeauto.db.core;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.Statement;

import org.openhab.ui.webapp.cloud.exception.CloudException;

public interface IDataBaseDAO {

	public Connection getNewConnection(String dbType)  throws CloudException;
	
	public void closeDbObjects(Statement statempent, ResultSet resultSet,Connection connection)  throws CloudException;
	
	public void closeStmtAndResultset(Statement statempent, ResultSet resultSet)  throws CloudException;
	
	public  void commit(Connection connection)  throws CloudException;
	
}
