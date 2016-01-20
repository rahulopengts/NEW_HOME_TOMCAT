package com.openhab.core.ruleaction;

import java.util.ArrayList;
import java.util.List;

import org.openhab.core.library.types.DecimalType;
import org.openhab.core.library.types.HSBType;
import org.openhab.core.library.types.IncreaseDecreaseType;
import org.openhab.core.library.types.OnOffType;
import org.openhab.core.library.types.OpenClosedType;
import org.openhab.core.library.types.PercentType;
import org.openhab.core.library.types.StopMoveType;
import org.openhab.core.library.types.StringType;
import org.openhab.core.library.types.UpDownType;
import org.openhab.core.types.Command;
import org.openhab.core.types.TypeParser;
import org.openhab.model.script.actions.BusEvent;
import org.openhab.ui.webapp.cloud.exception.CloudException;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.openhab.core.event.handler.EventManager;
import com.openhab.core.threadstore.CloudThreadLocalStorage;

public class DroolsBusEvent extends BusEvent {

	static private final Logger logger = LoggerFactory.getLogger(DroolsBusEvent.class);

	
	public static void sendCommand(String itemName, String commandString,String homeId) {
		
		System.out.println("\nDroolsBusEvent->sendCoammand->MasterData->"+Thread.currentThread().getId()+":MasterData:"+CloudThreadLocalStorage.getCloudMasterData());
		EventManager	eventManager	=	new EventManager();
		
			try {
				
				EventManager manager	=	new EventManager();
				Command command = getCommand(commandString);
				eventManager.postUpdate(itemName, command, "demo");

				System.out.println("\nDroolsBusEvent->Done");
				
			} catch (CloudException e) {
				logger.warn("Item '" + itemName + "' does not exist.");
			}
		
	}

	/**
	 * Convert a string representation of a command to an openHAB command.
	 * 
	 * @param value
	 *            string representation of command
	 * @return Command
	 */
	protected static Command getCommand(String value) {

		List<Class<? extends Command>> commandList = new ArrayList<Class<? extends Command>>();

		commandList.add(OnOffType.class);
		commandList.add(OpenClosedType.class);
		commandList.add(UpDownType.class);
		commandList.add(IncreaseDecreaseType.class);
		commandList.add(StopMoveType.class);
		commandList.add(HSBType.class);
		commandList.add(PercentType.class);
		commandList.add(DecimalType.class);
		commandList.add(StringType.class);

		return TypeParser.parseCommand(commandList, value);

	}
}
