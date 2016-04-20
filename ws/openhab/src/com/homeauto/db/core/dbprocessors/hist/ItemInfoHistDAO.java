package com.homeauto.db.core.dbprocessors.hist;

import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.SQLException;
import java.util.HashMap;
import java.util.Iterator;

import org.openhab.core.items.Item;
import org.openhab.core.items.ItemRegistry;
import org.openhab.ui.webapp.cloud.exception.CloudException;

import com.homeauto.db.core.AbstractDatabaseDAO;
import com.homeauto.db.core.DatabaseUtil;
import com.homeauto.db.core.dbprocessors.SiteQueryConstants;

public class ItemInfoHistDAO extends AbstractDatabaseDAO implements ItemInfoHistColumnConstants ,SiteQueryConstants{

	
	
	public HashMap<String, String> addItemInfoHistory(String userSiteid,String primaryKey_siteId,Item item) throws CloudException {
		int siteId	=	1;
		PreparedStatement statement	=	null;
		Connection connection	=	null;
		try{
			connection	=	getNewConnection(DB_VENDOR);
			String	query	=	INSERT_ITEM_INFO_HIST; 
			statement	=	connection.prepareStatement(query);
			java.sql.Date sqlDate = new java.sql.Date(new java.util.Date().getTime());
			String itemName_Column	=	item.getName();
			String	itemInfo[]	=	DatabaseUtil.getItemStateValueForUpdate(item);
			String itemValue_Column	=	itemInfo[0];	
			statement.setInt(1, siteId);
			statement.setString(2, itemName_Column);
			statement.setString(3, itemValue_Column);
			statement.setString(4, itemInfo[1]);
			statement.setDate(5,sqlDate);
			statement.execute();
			commit(connection);
		} catch (SQLException e){
			e.printStackTrace();
		} finally {
			closeDbObjects(statement, null, connection);
		}
		
		return null;
	}

}
