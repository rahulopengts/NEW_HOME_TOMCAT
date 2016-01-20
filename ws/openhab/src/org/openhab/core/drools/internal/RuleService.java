/**
 * Copyright (c) 2010-2013, openHAB.org and others.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 */
package org.openhab.core.drools.internal;

import com.google.common.base.Throwables;
import com.google.common.collect.LinkedListMultimap;
import org.apache.commons.io.FileUtils;
import org.apache.commons.io.filefilter.FileFilterUtils;
import org.apache.commons.io.filefilter.HiddenFileFilter;
import org.apache.commons.io.filefilter.IOFileFilter;
import org.apache.commons.io.monitor.FileAlterationListenerAdaptor;
import org.apache.commons.io.monitor.FileAlterationMonitor;
import org.apache.commons.io.monitor.FileAlterationObserver;
import org.apache.commons.lang.time.DateFormatUtils;
import org.drools.core.io.impl.ClassPathResource;
import org.drools.core.io.impl.FileSystemResource;
import org.kie.api.KieServices;
import org.kie.api.builder.KieFileSystem;
import org.kie.api.builder.Message;
import org.kie.api.builder.ReleaseId;
import org.kie.api.builder.Results;
import org.kie.api.builder.model.KieModuleModel;
import org.kie.api.builder.model.KieSessionModel;
import org.kie.api.conf.EventProcessingOption;
import org.kie.api.io.Resource;
import org.kie.api.runtime.KieContainer;
import org.kie.api.runtime.KieSession;
import org.kie.api.runtime.rule.FactHandle;
import org.openhab.core.drools.event.*;
import org.openhab.core.items.*;
import org.openhab.core.types.Command;
import org.openhab.core.types.EventType;
import org.openhab.core.types.State;
import org.osgi.service.event.Event;
import org.osgi.service.event.EventHandler;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.File;
import java.util.*;

import static org.openhab.core.events.EventConstants.TOPIC_PREFIX;
import static org.openhab.core.events.EventConstants.TOPIC_SEPERATOR;

public class RuleService implements EventHandler, ItemRegistryChangeListener, StateChangeListener {

    private static final File RULES_DIR = new File("configurations/drools/");
    private static final IOFileFilter RULES_FILTER = FileFilterUtils.and(
            FileFilterUtils.fileFileFilter(),
            HiddenFileFilter.VISIBLE);
    private static final IOFileFilter RULES_DIR_FILTER = FileFilterUtils.and(
            FileFilterUtils.directoryFileFilter(),
            HiddenFileFilter.VISIBLE);

    private static final Logger logger = LoggerFactory.getLogger(RuleService.class);

    private ItemRegistry itemRegistry;

    private Map<String, FactHandle> factHandles = new HashMap<String, FactHandle>();

    private final LinkedListMultimap<String, Object> updates = LinkedListMultimap.create();

    private KieServices ks;

    private KieFileSystem kfs;

    private KieContainer container;

    private KieSession session;

    private FileAlterationMonitor monitor;
    private RuleService.RuleChangeListener ruleChangeListener = new RuleChangeListener();
    private Map<String, Resource> changedRules = Collections.synchronizedMap(new HashMap<String, Resource>());

    private ExecutionThread thread;

    public void activate() {
        ks = KieServices.Factory.get();
        kfs = ks.newKieFileSystem();
        KieModuleModel moduleModel = ks.newKieModuleModel();
        moduleModel.newKieBaseModel("openhab-rules")
                .setEventProcessingMode(EventProcessingOption.STREAM)
                .newKieSessionModel("openhab-session")
                .setType(KieSessionModel.KieSessionType.STATEFUL)
                .setConsoleLogger("org.openhab.core.drools");
        kfs.writeKModuleXML(moduleModel.toXML());

        ReleaseId releaseId = loadRules(Collections.<String, Resource>singletonMap(
                "core/default.drl", new ClassPathResource("/org/openhab/core/drools/default.drl")));
        if (releaseId == null) {
            throw new IllegalStateException("Initial rule loading failed");
        }
        container = ks.newKieContainer(releaseId);
        for (File file : listAllFiles()) {
            ruleChangeListener.onFileCreate(file);
        }

        session = container.newKieSession("openhab-session");

        thread = new ExecutionThread();
        addItems(itemRegistry.getItems());
        addUpdate(new SystemStartup());
        thread.start();

        try {
            FileAlterationObserver observer = new FileAlterationObserver(RULES_DIR,
                    FileFilterUtils.or(RULES_FILTER, RULES_DIR_FILTER));
            observer.addListener(ruleChangeListener);
            monitor = new FileAlterationMonitor();
            monitor.addObserver(observer);
            monitor.start();
        } catch (Exception e) {
            throw Throwables.propagate(e);
        }

        logger.info("Dools service started");
    }

    private boolean active() {
        ExecutionThread thread = this.thread;
        return thread != null && !thread.shutdown;
    }

    private Collection<File> listAllFiles() {
        if (!RULES_DIR.exists()) {
            throw new IllegalStateException("Drools folder not found: " + RULES_DIR.getAbsolutePath());
        }
        return FileUtils.listFiles(RULES_DIR, RULES_FILTER, RULES_DIR_FILTER);
    }

    private String formatMessages(List<Message> messages) {
        StringBuilder builder = new StringBuilder("Drools compilation messages:");
        for (Message message : messages) {
            builder.append('\n').append(message.toString());
        }
        return builder.toString();
    }

    private ReleaseId loadRules(Map<String, Resource> rules) {
        ReleaseId releaseId = ks.newReleaseId("org.openhab", "drools", DateFormatUtils.ISO_DATETIME_FORMAT.format(new Date()));
        kfs.generateAndWritePomXML(releaseId);
        for (Map.Entry<String, Resource> rule : rules.entrySet()) {
            String path = "src/main/resources/" + rule.getKey();
            Resource resource = rule.getValue();
            if (resource != null) {
                kfs.write(path, resource);
            } else {
                kfs.delete(path);
            }
        }
        try {
            Results results = ks.newKieBuilder(kfs).buildAll().getResults();
            if (results.hasMessages(Message.Level.ERROR)) {
                releaseId = null;
                logger.error(formatMessages(results.getMessages(Message.Level.ERROR)));
            }
            if (results.hasMessages(Message.Level.WARNING)) {
                logger.warn(formatMessages(results.getMessages(Message.Level.WARNING)));
            }
            if (results.hasMessages(Message.Level.INFO)) {
                logger.info(formatMessages(results.getMessages(Message.Level.INFO)));
            }
            if (releaseId != null) {
                logger.info("Loaded Drools knowledge base " + releaseId.toString());
            }
            return releaseId;
        } catch (Exception e) {
            logger.error("Unexpected error during Drools compilation", e);
            return null;
        }
    }

    public void deactivate() {
        if (monitor != null) {
            try {
                monitor.stop();
            } catch (Exception e) {
                logger.error("Cannot stop rule change monitor", e);
            }
            monitor = null;
        }
        changedRules.clear();

        if (active()) {
            addUpdate(new SystemShutdown());
            try {
                thread.join();
            } catch (InterruptedException e) {
                thread.interrupt();
                logger.error("Drools thread shutdown interrupted", e);
            }
            thread = null;
        }

        if (session != null) {
            try {
                session.halt();
                session.dispose();
            } catch (Exception e) {
                logger.error("Cannot stop rule change monitor", e);
            }
            session = null;
        }
        container = null;
        kfs = null;
        ks = null;

        factHandles.clear();
        updates.clear();

        logger.info("Dools service stopped");
    }

    public void setItemRegistry(ItemRegistry itemRegistry) {
        this.itemRegistry = itemRegistry;
        itemRegistry.addItemRegistryChangeListener(this);
    }

    public void unsetItemRegistry(ItemRegistry itemRegistry) {
        itemRegistry.removeItemRegistryChangeListener(this);
        this.itemRegistry = null;
    }

    /**
     * {@inheritDoc}
     */
    public void allItemsChanged(Collection<String> oldItemNames) {
        addItems(itemRegistry.getItems());
    }

    /**
     * {@inheritDoc}
     */
    public void itemAdded(Item item) {
        addItems(Collections.singleton(item));
    }

    private void addItems(Collection<Item> items) {
        if (active()) {
            synchronized (updates) {
                for (Item item : items) {
                    replaceUpdate("item=" + item.getName(), item);
                    if (item instanceof GenericItem) {
                        ((GenericItem) item).addStateChangeListener(this);
                    }
                }
            }
        }
    }

    /**
     * {@inheritDoc}
     */
    public void itemRemoved(Item item) {
        if (active()) {
            if (item instanceof GenericItem) {
                ((GenericItem) item).removeStateChangeListener(this);
            }
            replaceUpdate("item=" + item.getName(), null);
        }
    }

    /**
     * {@inheritDoc}
     */
    public void stateChanged(Item item, State oldState, State newState) {
        if (active()) {
            addUpdate("state=" + item.getName(), StateEvent.create(item, oldState, newState));
            replaceUpdate("item=" + item.getName(), item);
        }
    }

    /**
     * {@inheritDoc}
     */
    public void stateUpdated(Item item, State state) {
        stateChanged(item, state, state);
    }

    public void receiveCommand(String itemName, Command command) {
        try {
            Item item = itemRegistry.getItem(itemName);
            addUpdate("command=" + item.getName(), new CommandEvent(item, command));
        } catch (ItemNotFoundException e) {
            logger.error("Received command {} for unknown item {}", command, itemName);
        }
    }

    /**
     * {@inheritDoc}
     */
    public void handleEvent(Event event) {
        String itemName = (String) event.getProperty("item");

        String topic = event.getTopic();
        String[] topicParts = topic.split(TOPIC_SEPERATOR);

        if (!(topicParts.length > 2) || !topicParts[0].equals(TOPIC_PREFIX)) {
            return; // we have received an event with an invalid topic
        }
        String operation = topicParts[1];

        if (operation.equals(EventType.COMMAND.toString())) {
            Command command = (Command) event.getProperty("command");
            if (command != null) receiveCommand(itemName, command);
        }
    }

    private void execute() {
        List<Map.Entry<String, Object>> updates;
        synchronized (this.updates) {
            updates = new ArrayList<Map.Entry<String, Object>>(this.updates.entries());
            this.updates.clear();
        }

        List<FactHandle> immediate = new LinkedList<FactHandle>();
        for (Map.Entry<String, Object> update : updates) {
            String key = update.getKey();
            Object value = update.getValue();
            FactHandle handle;
            if (key == null) {
                handle = session.insert(value);
            } else {
                handle = factHandles.get(key);
                if (value == null) {
                    if (handle != null) {
                        session.delete(handle);
                    }
                } else {
                    if (handle == null) {
                        handle = session.insert(value);
                        factHandles.put(key, handle);
                    } else {
                        session.update(handle, value);
                    }
                }
                if (value == null || value instanceof ImmediateEvent) {
                    factHandles.remove(key);
                }
            }

            if (value instanceof ImmediateEvent) {
                immediate.add(handle);
            }
            if (value instanceof SystemShutdown) {
                thread.shutdown = true;
            }
        }

        try {
            session.fireAllRules();
            
        } finally {
            for (FactHandle handle : immediate) {
                session.delete(handle);
            }
        }
    }

    private void addUpdate(Object value) {
        addUpdate(null, value);
    }

    private void addUpdate(String key, Object value) {
        synchronized (updates) {
            updates.put(key, value);
        }
    }

    private void replaceUpdate(String key, Object value) {
        synchronized (updates) {
            updates.replaceValues(key, Collections.singleton(value));
        }
    }

    private class ExecutionThread extends Thread {
        boolean shutdown = false;

        @Override
        public void run() {
            while (!shutdown) {
                if (!changedRules.isEmpty()) {
                    try {
                        HashMap<String, Resource> rules = new HashMap<String, Resource>(changedRules);
                        ReleaseId releaseId = loadRules(rules);
                        changedRules.keySet().removeAll(rules.keySet());
                        if (releaseId != null) {
                            container.updateToVersion(releaseId);
                        }
                    } catch (Exception e) {
                        logger.error("Unexpected error during Drools reload", e);
                    }
                }
                try {
                    synchronized (updates) {
                        if (updates.isEmpty()) {
                            updates.wait(1000l);
                        }
                    }
                    execute();
                } catch (InterruptedException e) {
                    logger.error("Drools thread interrupted", e);
                    return;
                }
            }
        }
    }

    private class RuleChangeListener extends FileAlterationListenerAdaptor {
        @Override
        public void onFileCreate(File file) {
            onFileChange(file);
        }

        @Override
        public void onFileDelete(File file) {
            onFileChange(file);
        }

        @Override
        public void onFileChange(File file) {
            String name = RULES_DIR.toURI().relativize(file.toURI()).getPath();
            if (file.exists() && file.isFile()) {
                changedRules.put(name, new FileSystemResource(file));
            } else {
                changedRules.put(name, null);
            }
        }
    }
}
