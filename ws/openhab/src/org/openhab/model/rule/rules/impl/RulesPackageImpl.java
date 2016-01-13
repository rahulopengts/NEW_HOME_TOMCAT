/**
 */
package org.openhab.model.rule.rules.impl;

import org.eclipse.emf.ecore.EAttribute;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.EPackage;
import org.eclipse.emf.ecore.EReference;

import org.eclipse.emf.ecore.impl.EPackageImpl;

import org.eclipse.xtext.xbase.XbasePackage;

import org.openhab.model.rule.rules.ChangedEventTrigger;
import org.openhab.model.rule.rules.CommandEventTrigger;
import org.openhab.model.rule.rules.EventTrigger;
import org.openhab.model.rule.rules.Import;
import org.openhab.model.rule.rules.Rule;
import org.openhab.model.rule.rules.RuleModel;
import org.openhab.model.rule.rules.RulesFactory;
import org.openhab.model.rule.rules.RulesPackage;
import org.openhab.model.rule.rules.SystemOnShutdownTrigger;
import org.openhab.model.rule.rules.SystemOnStartupTrigger;
import org.openhab.model.rule.rules.SystemTrigger;
import org.openhab.model.rule.rules.TimerTrigger;
import org.openhab.model.rule.rules.UpdateEventTrigger;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model <b>Package</b>.
 * <!-- end-user-doc -->
 * @generated
 */
public class RulesPackageImpl extends EPackageImpl implements RulesPackage
{
  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  private EClass ruleModelEClass = null;

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  private EClass importEClass = null;

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  private EClass ruleEClass = null;

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  private EClass eventTriggerEClass = null;

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  private EClass commandEventTriggerEClass = null;

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  private EClass updateEventTriggerEClass = null;

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  private EClass changedEventTriggerEClass = null;

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  private EClass timerTriggerEClass = null;

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  private EClass systemTriggerEClass = null;

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  private EClass systemOnStartupTriggerEClass = null;

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  private EClass systemOnShutdownTriggerEClass = null;

  /**
   * Creates an instance of the model <b>Package</b>, registered with
   * {@link org.eclipse.emf.ecore.EPackage.Registry EPackage.Registry} by the package
   * package URI value.
   * <p>Note: the correct way to create the package is via the static
   * factory method {@link #init init()}, which also performs
   * initialization of the package, or returns the registered package,
   * if one already exists.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see org.eclipse.emf.ecore.EPackage.Registry
   * @see org.openhab.model.rule.rules.RulesPackage#eNS_URI
   * @see #init()
   * @generated
   */
  private RulesPackageImpl()
  {
    super(eNS_URI, RulesFactory.eINSTANCE);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  private static boolean isInited = false;

  /**
   * Creates, registers, and initializes the <b>Package</b> for this model, and for any others upon which it depends.
   * 
   * <p>This method is used to initialize {@link RulesPackage#eINSTANCE} when that field is accessed.
   * Clients should not invoke it directly. Instead, they should simply access that field to obtain the package.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see #eNS_URI
   * @see #createPackageContents()
   * @see #initializePackageContents()
   * @generated
   */
  public static RulesPackage init()
  {
    if (isInited) return (RulesPackage)EPackage.Registry.INSTANCE.getEPackage(RulesPackage.eNS_URI);

    // Obtain or create and register package
    RulesPackageImpl theRulesPackage = (RulesPackageImpl)(EPackage.Registry.INSTANCE.get(eNS_URI) instanceof RulesPackageImpl ? EPackage.Registry.INSTANCE.get(eNS_URI) : new RulesPackageImpl());

    isInited = true;

    // Initialize simple dependencies
    XbasePackage.eINSTANCE.eClass();

    // Create package meta-data objects
    theRulesPackage.createPackageContents();

    // Initialize created meta-data
    theRulesPackage.initializePackageContents();

    // Mark meta-data to indicate it can't be changed
    theRulesPackage.freeze();

  
    // Update the registry and return the package
    EPackage.Registry.INSTANCE.put(RulesPackage.eNS_URI, theRulesPackage);
    return theRulesPackage;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EClass getRuleModel()
  {
    return ruleModelEClass;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EReference getRuleModel_Imports()
  {
    return (EReference)ruleModelEClass.getEStructuralFeatures().get(0);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EReference getRuleModel_Variables()
  {
    return (EReference)ruleModelEClass.getEStructuralFeatures().get(1);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EReference getRuleModel_Rules()
  {
    return (EReference)ruleModelEClass.getEStructuralFeatures().get(2);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EClass getImport()
  {
    return importEClass;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EAttribute getImport_ImportedNamespace()
  {
    return (EAttribute)importEClass.getEStructuralFeatures().get(0);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EClass getRule()
  {
    return ruleEClass;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EAttribute getRule_Name()
  {
    return (EAttribute)ruleEClass.getEStructuralFeatures().get(0);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EReference getRule_Eventtrigger()
  {
    return (EReference)ruleEClass.getEStructuralFeatures().get(1);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EReference getRule_Script()
  {
    return (EReference)ruleEClass.getEStructuralFeatures().get(2);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EClass getEventTrigger()
  {
    return eventTriggerEClass;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EClass getCommandEventTrigger()
  {
    return commandEventTriggerEClass;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EAttribute getCommandEventTrigger_Item()
  {
    return (EAttribute)commandEventTriggerEClass.getEStructuralFeatures().get(0);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EAttribute getCommandEventTrigger_Command()
  {
    return (EAttribute)commandEventTriggerEClass.getEStructuralFeatures().get(1);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EClass getUpdateEventTrigger()
  {
    return updateEventTriggerEClass;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EAttribute getUpdateEventTrigger_Item()
  {
    return (EAttribute)updateEventTriggerEClass.getEStructuralFeatures().get(0);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EAttribute getUpdateEventTrigger_State()
  {
    return (EAttribute)updateEventTriggerEClass.getEStructuralFeatures().get(1);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EClass getChangedEventTrigger()
  {
    return changedEventTriggerEClass;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EAttribute getChangedEventTrigger_Item()
  {
    return (EAttribute)changedEventTriggerEClass.getEStructuralFeatures().get(0);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EAttribute getChangedEventTrigger_OldState()
  {
    return (EAttribute)changedEventTriggerEClass.getEStructuralFeatures().get(1);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EAttribute getChangedEventTrigger_NewState()
  {
    return (EAttribute)changedEventTriggerEClass.getEStructuralFeatures().get(2);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EClass getTimerTrigger()
  {
    return timerTriggerEClass;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EAttribute getTimerTrigger_Cron()
  {
    return (EAttribute)timerTriggerEClass.getEStructuralFeatures().get(0);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EAttribute getTimerTrigger_Time()
  {
    return (EAttribute)timerTriggerEClass.getEStructuralFeatures().get(1);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EClass getSystemTrigger()
  {
    return systemTriggerEClass;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EClass getSystemOnStartupTrigger()
  {
    return systemOnStartupTriggerEClass;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EClass getSystemOnShutdownTrigger()
  {
    return systemOnShutdownTriggerEClass;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public RulesFactory getRulesFactory()
  {
    return (RulesFactory)getEFactoryInstance();
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  private boolean isCreated = false;

  /**
   * Creates the meta-model objects for the package.  This method is
   * guarded to have no affect on any invocation but its first.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public void createPackageContents()
  {
    if (isCreated) return;
    isCreated = true;

    // Create classes and their features
    ruleModelEClass = createEClass(RULE_MODEL);
    createEReference(ruleModelEClass, RULE_MODEL__IMPORTS);
    createEReference(ruleModelEClass, RULE_MODEL__VARIABLES);
    createEReference(ruleModelEClass, RULE_MODEL__RULES);

    importEClass = createEClass(IMPORT);
    createEAttribute(importEClass, IMPORT__IMPORTED_NAMESPACE);

    ruleEClass = createEClass(RULE);
    createEAttribute(ruleEClass, RULE__NAME);
    createEReference(ruleEClass, RULE__EVENTTRIGGER);
    createEReference(ruleEClass, RULE__SCRIPT);

    eventTriggerEClass = createEClass(EVENT_TRIGGER);

    commandEventTriggerEClass = createEClass(COMMAND_EVENT_TRIGGER);
    createEAttribute(commandEventTriggerEClass, COMMAND_EVENT_TRIGGER__ITEM);
    createEAttribute(commandEventTriggerEClass, COMMAND_EVENT_TRIGGER__COMMAND);

    updateEventTriggerEClass = createEClass(UPDATE_EVENT_TRIGGER);
    createEAttribute(updateEventTriggerEClass, UPDATE_EVENT_TRIGGER__ITEM);
    createEAttribute(updateEventTriggerEClass, UPDATE_EVENT_TRIGGER__STATE);

    changedEventTriggerEClass = createEClass(CHANGED_EVENT_TRIGGER);
    createEAttribute(changedEventTriggerEClass, CHANGED_EVENT_TRIGGER__ITEM);
    createEAttribute(changedEventTriggerEClass, CHANGED_EVENT_TRIGGER__OLD_STATE);
    createEAttribute(changedEventTriggerEClass, CHANGED_EVENT_TRIGGER__NEW_STATE);

    timerTriggerEClass = createEClass(TIMER_TRIGGER);
    createEAttribute(timerTriggerEClass, TIMER_TRIGGER__CRON);
    createEAttribute(timerTriggerEClass, TIMER_TRIGGER__TIME);

    systemTriggerEClass = createEClass(SYSTEM_TRIGGER);

    systemOnStartupTriggerEClass = createEClass(SYSTEM_ON_STARTUP_TRIGGER);

    systemOnShutdownTriggerEClass = createEClass(SYSTEM_ON_SHUTDOWN_TRIGGER);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  private boolean isInitialized = false;

  /**
   * Complete the initialization of the package and its meta-model.  This
   * method is guarded to have no affect on any invocation but its first.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public void initializePackageContents()
  {
    if (isInitialized) return;
    isInitialized = true;

    // Initialize package
    setName(eNAME);
    setNsPrefix(eNS_PREFIX);
    setNsURI(eNS_URI);

    // Obtain other dependent packages
    XbasePackage theXbasePackage = (XbasePackage)EPackage.Registry.INSTANCE.getEPackage(XbasePackage.eNS_URI);

    // Create type parameters

    // Set bounds for type parameters

    // Add supertypes to classes
    commandEventTriggerEClass.getESuperTypes().add(this.getEventTrigger());
    updateEventTriggerEClass.getESuperTypes().add(this.getEventTrigger());
    changedEventTriggerEClass.getESuperTypes().add(this.getEventTrigger());
    timerTriggerEClass.getESuperTypes().add(this.getEventTrigger());
    systemTriggerEClass.getESuperTypes().add(this.getEventTrigger());
    systemOnStartupTriggerEClass.getESuperTypes().add(this.getSystemTrigger());
    systemOnShutdownTriggerEClass.getESuperTypes().add(this.getSystemTrigger());

    // Initialize classes and features; add operations and parameters
    initEClass(ruleModelEClass, RuleModel.class, "RuleModel", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
    initEReference(getRuleModel_Imports(), this.getImport(), null, "imports", null, 0, -1, RuleModel.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
    initEReference(getRuleModel_Variables(), theXbasePackage.getXExpression(), null, "variables", null, 0, -1, RuleModel.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
    initEReference(getRuleModel_Rules(), this.getRule(), null, "rules", null, 0, -1, RuleModel.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

    initEClass(importEClass, Import.class, "Import", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(getImport_ImportedNamespace(), ecorePackage.getEString(), "importedNamespace", null, 0, 1, Import.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

    initEClass(ruleEClass, Rule.class, "Rule", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(getRule_Name(), ecorePackage.getEString(), "name", null, 0, 1, Rule.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
    initEReference(getRule_Eventtrigger(), this.getEventTrigger(), null, "eventtrigger", null, 0, -1, Rule.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
    initEReference(getRule_Script(), theXbasePackage.getXBlockExpression(), null, "script", null, 0, 1, Rule.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

    initEClass(eventTriggerEClass, EventTrigger.class, "EventTrigger", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);

    initEClass(commandEventTriggerEClass, CommandEventTrigger.class, "CommandEventTrigger", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(getCommandEventTrigger_Item(), ecorePackage.getEString(), "item", null, 0, 1, CommandEventTrigger.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
    initEAttribute(getCommandEventTrigger_Command(), ecorePackage.getEString(), "command", null, 0, 1, CommandEventTrigger.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

    initEClass(updateEventTriggerEClass, UpdateEventTrigger.class, "UpdateEventTrigger", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(getUpdateEventTrigger_Item(), ecorePackage.getEString(), "item", null, 0, 1, UpdateEventTrigger.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
    initEAttribute(getUpdateEventTrigger_State(), ecorePackage.getEString(), "state", null, 0, 1, UpdateEventTrigger.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

    initEClass(changedEventTriggerEClass, ChangedEventTrigger.class, "ChangedEventTrigger", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(getChangedEventTrigger_Item(), ecorePackage.getEString(), "item", null, 0, 1, ChangedEventTrigger.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
    initEAttribute(getChangedEventTrigger_OldState(), ecorePackage.getEString(), "oldState", null, 0, 1, ChangedEventTrigger.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
    initEAttribute(getChangedEventTrigger_NewState(), ecorePackage.getEString(), "newState", null, 0, 1, ChangedEventTrigger.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

    initEClass(timerTriggerEClass, TimerTrigger.class, "TimerTrigger", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(getTimerTrigger_Cron(), ecorePackage.getEString(), "cron", null, 0, 1, TimerTrigger.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
    initEAttribute(getTimerTrigger_Time(), ecorePackage.getEString(), "time", null, 0, 1, TimerTrigger.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

    initEClass(systemTriggerEClass, SystemTrigger.class, "SystemTrigger", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);

    initEClass(systemOnStartupTriggerEClass, SystemOnStartupTrigger.class, "SystemOnStartupTrigger", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);

    initEClass(systemOnShutdownTriggerEClass, SystemOnShutdownTrigger.class, "SystemOnShutdownTrigger", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);

    // Create resource
    createResource(eNS_URI);
  }

} //RulesPackageImpl
