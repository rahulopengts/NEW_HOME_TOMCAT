package com.sample.app;
 
import java.util.ArrayList;
import java.util.List;
 
public class Event {
 
    static List<String> events = new ArrayList();
 
    public static List<String> getEvents() {
        return events;
    }
 
    public static void add(String log) {
        events.add(log);
    }
}