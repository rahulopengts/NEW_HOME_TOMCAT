package com.homeauto.db.core;

import java.util.Calendar;
import java.util.Iterator;

import org.openhab.core.items.GenericItem;
import org.openhab.core.items.Item;
import org.openhab.core.items.ItemRegistry;
import org.openhab.core.library.items.ColorItem;
import org.openhab.core.library.items.ContactItem;
import org.openhab.core.library.items.DateTimeItem;
import org.openhab.core.library.items.DimmerItem;
import org.openhab.core.library.items.NumberItem;
import org.openhab.core.library.items.RollershutterItem;
import org.openhab.core.library.items.SwitchItem;
import org.openhab.core.library.types.DateTimeType;
import org.openhab.core.library.types.DecimalType;
import org.openhab.core.library.types.HSBType;
import org.openhab.core.library.types.OnOffType;
import org.openhab.core.library.types.OpenClosedType;
import org.openhab.core.library.types.StringType;
import org.openhab.core.types.State;

public class DatabaseUtil {


	public static final String	DEFAULT_DECIMAL_VALUE	=	"0.0";
	
	public static final String	ColorItem         	=		"HSBType";//       CHAR(25)
	public static final String	ContactItem       	=		"OnOffType";//     CHAR(6)
	public static final String	DateTimeType  		=		"DATETIME";//
	public static final String	PercentType   		=		"TINYINT";//
	public static final String	NumberItem        	=		"DecimalType";//   DOUBLE
	public static final String	RollershutterItem 	=		"PercentType";//   TINYINT
	public static final String	StringItem        	=		"StringType";//    VARCHAR(20000)
	public static final String	SwitchItem        	=		"OnOffType";//     CHAR(3)

	public static String[] getItemStateValue(Item item){
		String	value[]	=	new String[2];
		String dataType	=	"STRING";
		if (item instanceof DimmerItem || item instanceof RollershutterItem) {
/*			State	state	=	item.getStateAs(PercentType.class);
			if(state==null){
				value[0]	=	"0.0";
			} else {
				value[0] = item.getStateAs(PercentType.class).toString();
			}
*/			
			value[0]	=	"0";	
			dataType	=	PercentType;
		} else if (item instanceof ColorItem) {
/*			State	state	=	item.getStateAs(HSBType.class);
			if(state==null){
				value[0]	=	DEFAULT_DECIMAL_VALUE;
			} else {
				value[0] = item.getStateAs(HSBType.class).toString();
			}
*/			
			value[0]	=	HSBType.RED.toString();
			dataType	=	ColorItem;
		} else if(item instanceof SwitchItem){ 
/*			State	state	=	item.getStateAs(OnOffType.class);
			if(state==null){
				value[0]	=	OnOffType.OFF.toString();
			} else {
				value[0] = item.getStateAs(OnOffType.class).toString();
			}
*/			
			value[0]	=	OnOffType.OFF.toString();
			dataType	=	SwitchItem;
		
		} else if(item instanceof NumberItem){ 
/*			State	state	=	item.getStateAs(DecimalType.class);
			if(state==null){
				value[0]	=	DecimalType.ZERO.toString();
			} else {
				value[0] = item.getStateAs(DecimalType.class).toString();
			}
*/			
			
			
			value[0]	=	DecimalType.ZERO.toString();
			dataType	=	NumberItem;
		
		} else if(item instanceof ContactItem){
			
//			State	state	=	item.getStateAs(OpenClosedType.class);
//			
//			if(state==null){
//				value[0]	=	OpenClosedType.OPEN.toString();
//			} else {
//				value[0] = item.getStateAs(OpenClosedType.class).toString();
//			}
			
			value[0]	=	OpenClosedType.OPEN.toString();
			dataType	=	ContactItem;
		
		
		} else if(item instanceof DateTimeItem){
			Calendar calendar = Calendar.getInstance();
			//calendar.setTimeInMillis(rs.getTimestamp(2).getTime());
			State	state	=	new DateTimeType(calendar);

			value[0] = state.toString();
			dataType	=	DateTimeType;
		
		}	else {
			// All other items should return the best format by default
			//item.getStateAs(StringType.class).toString();
			State	state	=	item.getState();
			if(state==null){
				value[0]	=	"1.0";
			} else {
				value[0] = item.getState().toString();
			}
			
			//value[0] = item.getStateAs(StringType.class).toString();
			dataType	=	StringItem;
		}
		System.out.println("\nDatabaseUtili->getItemstateValue->"+item.getName()+":Value:"+value);
		value[1]	=	dataType;
		return value;
	}


	public static String[] getItemStateValueForUpdate(Item item){
		String	value[]	=	new String[2];
		String dataType	=	"STRING";
		if (item instanceof DimmerItem || item instanceof RollershutterItem) {
				value[0] = item.getStateAs(DecimalType.class).toString();
		} else if (item instanceof ColorItem) {
				value[0] = item.getStateAs(HSBType.class).toString();
		} else if(item instanceof SwitchItem){ 
				value[0] = item.getStateAs(OnOffType.class).toString();
		
		} else if(item instanceof NumberItem){ 
				value[0] = item.getStateAs(DecimalType.class).toString();
		
		} else if(item instanceof ContactItem){
			
				value[0] = item.getStateAs(OpenClosedType.class).toString();
		
		
		} else if(item instanceof DateTimeItem){
			Calendar calendar = Calendar.getInstance();
			//calendar.setTimeInMillis(rs.getTimestamp(2).getTime());
			State	state	=	new DateTimeType(calendar);

			value[0] = state.toString();
			dataType	=	DateTimeType;
		
		}	else {
			// All other items should return the best format by default
			//item.getStateAs(StringType.class).toString();
				value[0] = item.getState().toString();
		}
		System.out.println("\nDatabaseUtili->getItemstateValue->"+item.getName()+":Value:"+value);
		value[1]	=	dataType;
		return value;
	}

	
	private static void mapItemStateValue(Item item,String stateValue,String dataType){
		String	value	=	null;
		State state	=	item.getState();
		
		if (item instanceof DimmerItem || item instanceof RollershutterItem) {
			state = new DecimalType(Double.parseDouble(stateValue));
		} else if (item instanceof ColorItem) {
			state = new HSBType(stateValue);
			
		} else if(item instanceof SwitchItem){ 
			state = OnOffType.valueOf(stateValue);
			((GenericItem)(item)).setState(state);
			System.out.println("\nDatabaseUtili->Setting Value for->"+state);
		} else if(item instanceof NumberItem){ 
			state = new DecimalType(Double.parseDouble(stateValue));
		
		} else if(item instanceof ContactItem){
			state = OpenClosedType.valueOf(stateValue);
		} else if(item instanceof DateTimeItem){
		}	else {
			state = new StringType(stateValue);
		}
		
		System.out.println("\nDatabaseUtili->getItemstateValue->state->"+item.getName()+":Value:"+state);
	}

	
    public static void setItemState(String nodeId,String nodeStateValue,String dataType,ItemRegistry itemRegistry){            
		Iterable<Item> itemRegistryIterable	=	itemRegistry.getItems();
		Iterator<Item>	itemIterator		=	itemRegistryIterable.iterator();
    	while(itemIterator.hasNext()){
			Item item	=	itemIterator.next();
			String itemName_Column	=	item.getName();
			if(itemName_Column.equals(nodeId)){
				mapItemStateValue(item,nodeStateValue,dataType);
			}
//			String	itemInfo[]	=	DatabaseUtil.getItemStateValue(item);; 
//			String itemValue_Column	=	itemInfo[0];	
		}
    }

}
