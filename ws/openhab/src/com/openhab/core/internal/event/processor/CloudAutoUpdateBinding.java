package com.openhab.core.internal.event.processor;

import org.openhab.core.autoupdate.AutoUpdateBindingProvider;
import org.openhab.core.binding.AbstractBinding;
import org.openhab.core.items.GenericItem;
import org.openhab.core.items.ItemNotFoundException;
import org.openhab.core.items.ItemRegistry;
import org.openhab.core.types.Command;
import org.openhab.core.types.State;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class CloudAutoUpdateBinding extends CloudAbstractBinding {

	private static final Logger logger = LoggerFactory.getLogger(CloudAutoUpdateBinding.class);
	
	protected ItemRegistry itemRegistry;
	
	public void setItemRegistry(ItemRegistry itemRegistry) {
		this.itemRegistry = itemRegistry;
	}

	public void unsetItemRegistry(ItemRegistry itemRegistry) {
		this.itemRegistry = null;
	}
	

	/**
	 * <p>Iterates through all registered {@link AutoUpdateBindingProvider}s and
	 * checks whether an autoupdate configuration is available for <code>itemName</code>.</p>
	 * 
	 * <p>If there are more then one {@link AutoUpdateBindingProvider}s providing
	 * a configuration the results are combined by a logical <em>OR</em>. If no
	 * configuration is provided at all the autoupdate defaults to <code>true</code>
	 * and an update is posted for the corresponding {@link State}.</p> 
	 * 
	 * @param itemName the item for which to find an autoupdate configuration
	 * @param command the command being received and posted as {@link State}
	 * update if <code>command</code> is instance of {@link State} as well.
	 */
	@Override
	public void receiveCommand(String itemName, Command command) {
		System.out.println("\nCloudAutoUpdateBinding->receiveCommand->itemName->"+itemName+"->Command->"+command.getClass()+"->Provider->"+this);
		if(command instanceof State){	
			System.out.println("\nCloudAutoUpdateBinding->receiveCommand->Not an instacen of state");
			postUpdate(itemName, (State) command);
		}
	}

	private void postUpdate(String itemName, State newStatus) {
		System.out.println("\nCloudAutoUpdateBinding->postUpdate->itemRegistry->"+itemRegistry);
		if (itemRegistry != null) {
			
			try {
				GenericItem item = (GenericItem) itemRegistry.getItem(itemName);
				item.setState(newStatus);

//				boolean isAccepted = false;
//				if (item.getAcceptedDataTypes().contains(newStatus.getClass())) {
//					isAccepted = true;
//				} else {
//					// Look for class hierarchy
//					for (Class<? extends State> state : item.getAcceptedDataTypes()) {
//						try {
//							if (!state.isEnum() && state.newInstance().getClass().isAssignableFrom(newStatus.getClass())) {
//								isAccepted = true;
//								break;
//							}
//						} catch (InstantiationException e) {
//							logger.warn("InstantiationException on ", e.getMessage()); // Should never happen
//						} catch (IllegalAccessException e) {
//							logger.warn("IllegalAccessException on ", e.getMessage()); // Should never happen
//						}
//					}
//				}				
//				if (isAccepted) {
//					item.setState(newStatus);
//				} else {
//					logger.debug("Received update of a not accepted type ("	+ newStatus.getClass().getSimpleName() + ") for item " + itemName);
//				}
			} catch (ItemNotFoundException e) {
				logger.debug("Received update for non-existing item: {}", e.getMessage());
			}
		}
	}
	
}

