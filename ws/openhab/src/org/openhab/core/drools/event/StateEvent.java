/**
 * Copyright (c) 2010-2013, openHAB.org and others.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 */
package org.openhab.core.drools.event;

import org.openhab.core.items.Item;
import org.openhab.core.types.State;

/**
 * This class is used as a fact in rules to inform about received status updates on the openHAB event bus.
 *
 * @author Kai Kreuzer
 * @since 0.7.0
 *
 */
public class StateEvent implements ImmediateEvent {

    private Item item;
    private State oldState;
    private State state;
    private boolean changed;

    protected StateEvent(Item item, State oldState, State state, boolean changed) {
        this.item = item;
        this.oldState = oldState;
		this.state = state;
		this.changed = changed;
	}

    public static StateEvent create(Item item, State oldState, State newState) {
        if (oldState.equals(newState)) {
            return create(item, newState);
        } else {
            return new StateChange(item, oldState, newState);
        }
    }

    public static StateEvent create(Item item, State state) {
        return new StateEvent(item, state, state, false);
    }

    public Item getItem() {
        return item;
    }

    public State getOldState() {
		return oldState;
	}

	public State getState() {
		return state;
	}

    public boolean isChanged() {
        return changed;
    }
}
