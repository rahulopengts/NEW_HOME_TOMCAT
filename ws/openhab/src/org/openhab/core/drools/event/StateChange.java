package org.openhab.core.drools.event;

import org.openhab.core.items.Item;
import org.openhab.core.types.State;

/**
 * @author Vladimir Kulev
 */
public class StateChange extends StateEvent {
    protected StateChange(Item item, State oldState, State state) {
        super(item, oldState, state, true);
    }
}
