package com.homeauto.db.core.dbprocessors;

import java.sql.Connection;
import java.sql.Date;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.HashMap;
import java.util.Iterator;

import org.openhab.core.items.Item;
import org.openhab.core.items.ItemRegistry;
import org.openhab.ui.webapp.cloud.exception.CloudException;

import com.homeauto.db.core.AbstractDatabaseDAO;
import com.homeauto.db.core.DatabaseUtil;
import com.openhab.core.db.vo.IBaseVO;
import com.openhab.core.db.vo.SiteInfoVO;
import com.openhab.core.util.DataConverterUtil;

public class SiteDataDAO extends AbstractDatabaseDAO implements SiteQueryConstants,SiteInfoColumnConstants {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub

	}
	
	public SiteInfoVO getSiteInitializationData(String userSiteId,String primaryKey_siteId) throws CloudException {
		Connection connection		=	null;
    	PreparedStatement statement	=	null;
        ResultSet resultSet = null;
        int siteId	=	0;
        HashMap<String, Object> dataMap	=new HashMap();
        SiteInfoVO	siteinfoVO	=	new SiteInfoVO();
        
		if(isConnectionOpen){
			connection		=	getConnection();
		} else {
			connection		=	getNewConnection(DB_VENDOR);
		}
		
        try {
        	siteId	=	DataConverterUtil.covertToInt(primaryKey_siteId, -1);
            // Get Connection and Statement
        	String query = FETCH_SITE_INFO;//"select * from USER_INFO";
        	
            statement = connection.prepareStatement(query);
            statement.setString(1, userSiteId);
            resultSet = statement.executeQuery();
            //resultSet = statement.executeQuery(query);
            
            System.out.println("\n HSQLDB Data->");
            while (resultSet.next()) {
            	
            	siteinfoVO.setSITE_ID(resultSet.getInt(SITE_INFO_SITE_ID));
            	siteinfoVO.setUSER_SITE_ID(resultSet.getString(SITE_INFO_USER_SITE_ID));
            	siteinfoVO.setDESCRIPTION(resultSet.getString(SITE_INFO_DESCRIPTION));
            	siteinfoVO.setF_NAME(resultSet.getString(SITE_INFO_F_NAME));
            	siteinfoVO.setL_NAME(resultSet.getString(SITE_INFO_L_NAME));
            	siteinfoVO.setGENDER(resultSet.getString(SITE_INFO_GENDER));
            	siteinfoVO.setPHONE(resultSet.getString(SITE_INFO_PHONE));
            	siteinfoVO.setREGDATE(resultSet.getDate(SITE_INFO_REGDATE));
            	siteinfoVO.setUSER_GRP_ID(resultSet.getString(SITE_INFO_USER_GRP_ID));
            	siteinfoVO.setINITIALIZED(resultSet.getInt(SITE_INFO_INITIALIZED));
            	
            	System.out.println("\n HSQLDB Data->Read->");
            	System.out.println(siteinfoVO.getSITE_ID());
            	System.out.println(siteinfoVO.getF_NAME());
            	System.out.println(siteinfoVO.getL_NAME());
            	System.out.println(siteinfoVO.getDESCRIPTION());
            	System.out.println(siteinfoVO.getPHONE());
            }
        } catch (SQLException e) {
            e.printStackTrace();
        }finally {
        	closeDbObjects(statement, resultSet, connection);
        }
		return siteinfoVO;
	}

	public boolean updateSiteInitializationData(String siteUserId,String primaryKey_siteId) throws CloudException {
		Connection connection		=	null;
    	PreparedStatement statement	=	null;
        ResultSet resultSet = null;
        int siteId	=	0;
		if(isConnectionOpen){
			connection		=	getConnection();
		} else {
			connection		=	getNewConnection(DB_VENDOR);
		}
		
        try {
        	siteId	=	DataConverterUtil.covertToInt(primaryKey_siteId, -1);
            // Get Connection and Statement
        	String query = UPDATE_SITE_INFO_INITIALIZATION;//"select * from USER_INFO";
        	
            statement = connection.prepareStatement(query);
            statement.setInt(1, siteId);
            System.out.println("\n HSQLDB Data-> UPDATING SITE_INFO with ID:"+siteId+"-> primary ->"+primaryKey_siteId);
            statement.executeUpdate();
            //resultSet = statement.executeQuery(query);
            System.out.println("\n HSQLDB Data-> DONE WITH UPDATE");
        } catch (SQLException e) {
            e.printStackTrace();
        }finally {
        	closeDbObjects(statement, resultSet, connection);
        }
		return true;
		
	}
	
	public HashMap<String, String> fetchSiteItemInfo(String userSiteId,String primaryKey_siteId,ItemRegistry itemRegistry) throws CloudException{
	
		
    	Connection connection		=	null;
    	PreparedStatement statement	=	null;
        ResultSet resultSet = null;
        int siteId	=	0;
        
        try {
        	siteId	=	DataConverterUtil.covertToInt(primaryKey_siteId, -1);
            // Get Connection and Statement
        	String query = FETCH_ITEM_INFO;//"select * from USER_INFO";
        	connection = getNewConnection(DB_VENDOR);
            statement = connection.prepareStatement(query);
            statement.setInt(1, siteId);
            resultSet = statement.executeQuery();
            System.out.println("\n Derby DB "+connection);
            
            while (resultSet.next()) {
                
                System.out.println("\n Existing Data "+resultSet.getString(1) + resultSet.getString(2) + resultSet.getString(3));
                DatabaseUtil.setItemState(resultSet.getString(ItemInfoColumnConstants.ITEM_INFO_ITEM_ID),resultSet.getString(ItemInfoColumnConstants.ITEM_INFO_ITEM_VALUE),resultSet.getString(ItemInfoColumnConstants.ITEM_INFO_DATATYPE),itemRegistry);
            }
            
            
        } catch (SQLException e) {
            e.printStackTrace();
        }finally {
        	closeDbObjects(statement, resultSet, connection);
        }
		return null;
	}

	
	public HashMap<String, String> initilizeSiteItemInfo(String userSiteid,String primaryKey_siteId,ItemRegistry itemRegistry) throws CloudException {
		//INSERT INTO ITEM_INFO (SITE_ID,ITEM_ID,ITEM_VALUE,DATATYPE,LAST_UPDATE
		int siteId	=	1;
		try{
			Iterable<Item> itemRegistryIterable	=	itemRegistry.getItems();
			Iterator<Item>	itemIterator	=	itemRegistryIterable.iterator();
			Connection connection	=	getNewConnection(DB_VENDOR);
			String	query	=	INSERT_SITE_INFO; 
			PreparedStatement statement	=	connection.prepareStatement(query);
			java.sql.Date sqlDate = new java.sql.Date(new java.util.Date().getTime());
			while(itemIterator.hasNext()){
				Item item	=	itemIterator.next();
				String itemName_Column	=	item.getName();
				String	itemInfo[]	=	DatabaseUtil.getItemStateValue(item);; 
				String itemValue_Column	=	itemInfo[0];	
				statement.setInt(1, siteId);
				statement.setString(2, itemName_Column);
				statement.setString(3, itemValue_Column);
				statement.setString(4, itemInfo[1]);
				statement.setDate(5,sqlDate);
				statement.addBatch();
			}
			statement.executeBatch();
			commit(connection);
	} catch (SQLException e){
		e.printStackTrace();
	}
		
		return null;
	}


	public boolean updateItemInfo(String siteUserId,String primaryKey_siteId,Item item) throws CloudException {
		Connection connection		=	null;
    	PreparedStatement statement	=	null;
        ResultSet resultSet = null;
        int siteId	=	0;
		if(isConnectionOpen){
			connection		=	getConnection();
		} else {
			connection		=	getNewConnection(DB_VENDOR);
		}
		
        try {
        	siteId	=	DataConverterUtil.covertToInt(primaryKey_siteId, -1);
            // Get Connection and Statement
        	String query = UPDATE_ITEM_INFO;//"select * from USER_INFO";
        	String	itemInfo[]	=	DatabaseUtil.getItemStateValueForUpdate(item); 
        	
            statement = connection.prepareStatement(query);
            
            //UPDATE ITEM_INFO set ITEM_VALUE=? WHERE SITE_ID=? AND ITEM_ID=?
            statement.setString(1, itemInfo[0]);
            statement.setInt(2, siteId);
            statement.setString(3, item.getName());
            
            System.out.println("\n HSQLDB Data-> UPDATE_ITEM_INFO with site ID:"+siteId+"-> Node ->"+ item.getName()+"and node value->"+itemInfo[0]);
            statement.executeUpdate();
            //resultSet = statement.executeQuery(query);
            System.out.println("\n HSQLDB Data-> DONE WITH UPDATE");
        } catch (SQLException e) {
            e.printStackTrace();
        }finally {
        	closeDbObjects(statement, resultSet, connection);
        }
		return true;
		
	}


}
