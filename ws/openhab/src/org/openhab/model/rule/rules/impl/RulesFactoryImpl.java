/**
 */
package org.openhab.model.rule.rules.impl;

import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EPackage;

import org.eclipse.emf.ecore.impl.EFactoryImpl;

import org.eclipse.emf.ecore.plugin.EcorePlugin;

import org.openhab.model.rule.rules.*;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model <b>Factory</b>.
 * <!-- end-user-doc -->
 * @generated
 */
public class RulesFactoryImpl extends EFactoryImpl implements RulesFactory
{
  /**
   * Creates the default factory implementation.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public static RulesFactory init()
  {
    try
    {
      RulesFactory theRulesFactory = (RulesFactory)EPackage.Registry.INSTANCE.getEFactory("http://www.openhab.org/model/rule/Rules"); 
      if (theRulesFactory != null)
      {
        return theRulesFactory;
      }
    }
    catch (Exception exception)
    {
      EcorePlugin.INSTANCE.log(exception);
    }
    return new RulesFactoryImpl();
  }

  /**
   * Creates an instance of the factory.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public RulesFactoryImpl()
  {
    super();
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  @Override
  public EObject create(EClass eClass)
  {
    switch (eClass.getClassifierID())
    {
      case RulesPackage.RULE_MODEL: return createRuleModel();
      case RulesPackage.IMPORT: return createImport();
      case RulesPackage.RULE: return createRule();
      case RulesPackage.EVENT_TRIGGER: return createEventTrigger();
      case RulesPackage.COMMAND_EVENT_TRIGGER: return createCommandEventTrigger();
      case RulesPackage.UPDATE_EVENT_TRIGGER: return createUpdateEventTrigger();
      case RulesPackage.CHANGED_EVENT_TRIGGER: return createChangedEventTrigger();
      case RulesPackage.TIMER_TRIGGER: return createTimerTrigger();
      case RulesPackage.SYSTEM_TRIGGER: return createSystemTrigger();
      case RulesPackage.SYSTEM_ON_STARTUP_TRIGGER: return createSystemOnStartupTrigger();
      case RulesPackage.SYSTEM_ON_SHUTDOWN_TRIGGER: return createSystemOnShutdownTrigger();
      default:
        throw new IllegalArgumentException("The class '" + eClass.getName() + "' is not a valid classifier");
    }
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public RuleModel createRuleModel()
  {
    RuleModelImpl ruleModel = new RuleModelImpl();
    return ruleModel;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public Import createImport()
  {
    ImportImpl import_ = new ImportImpl();
    return import_;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public Rule createRule()
  {
    RuleImpl rule = new RuleImpl();
    return rule;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EventTrigger createEventTrigger()
  {
    EventTriggerImpl eventTrigger = new EventTriggerImpl();
    return eventTrigger;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public CommandEventTrigger createCommandEventTrigger()
  {
    CommandEventTriggerImpl commandEventTrigger = new CommandEventTriggerImpl();
    return commandEventTrigger;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public UpdateEventTrigger createUpdateEventTrigger()
  {
    UpdateEventTriggerImpl updateEventTrigger = new UpdateEventTriggerImpl();
    return updateEventTrigger;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public ChangedEventTrigger createChangedEventTrigger()
  {
    ChangedEventTriggerImpl changedEventTrigger = new ChangedEventTriggerImpl();
    return changedEventTrigger;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public TimerTrigger createTimerTrigger()
  {
    TimerTriggerImpl timerTrigger = new TimerTriggerImpl();
    return timerTrigger;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public SystemTrigger createSystemTrigger()
  {
    SystemTriggerImpl systemTrigger = new SystemTriggerImpl();
    return systemTrigger;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public SystemOnStartupTrigger createSystemOnStartupTrigger()
  {
    SystemOnStartupTriggerImpl systemOnStartupTrigger = new SystemOnStartupTriggerImpl();
    return systemOnStartupTrigger;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public SystemOnShutdownTrigger createSystemOnShutdownTrigger()
  {
    SystemOnShutdownTriggerImpl systemOnShutdownTrigger = new SystemOnShutdownTriggerImpl();
    return systemOnShutdownTrigger;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public RulesPackage getRulesPackage()
  {
    return (RulesPackage)getEPackage();
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @deprecated
   * @generated
   */
  @Deprecated
  public static RulesPackage getPackage()
  {
    return RulesPackage.eINSTANCE;
  }

} //RulesFactoryImpl
