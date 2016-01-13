/**
 */
package org.openhab.model.rule.rules.util;

import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EPackage;

import org.eclipse.emf.ecore.util.Switch;

import org.openhab.model.rule.rules.*;

/**
 * <!-- begin-user-doc -->
 * The <b>Switch</b> for the model's inheritance hierarchy.
 * It supports the call {@link #doSwitch(EObject) doSwitch(object)}
 * to invoke the <code>caseXXX</code> method for each class of the model,
 * starting with the actual class of the object
 * and proceeding up the inheritance hierarchy
 * until a non-null result is returned,
 * which is the result of the switch.
 * <!-- end-user-doc -->
 * @see org.openhab.model.rule.rules.RulesPackage
 * @generated
 */
public class RulesSwitch<T> extends Switch<T>
{
  /**
   * The cached model package
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  protected static RulesPackage modelPackage;

  /**
   * Creates an instance of the switch.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public RulesSwitch()
  {
    if (modelPackage == null)
    {
      modelPackage = RulesPackage.eINSTANCE;
    }
  }

  /**
   * Checks whether this is a switch for the given package.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @parameter ePackage the package in question.
   * @return whether this is a switch for the given package.
   * @generated
   */
  @Override
  protected boolean isSwitchFor(EPackage ePackage)
  {
    return ePackage == modelPackage;
  }

  /**
   * Calls <code>caseXXX</code> for each class of the model until one returns a non null result; it yields that result.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the first non-null result returned by a <code>caseXXX</code> call.
   * @generated
   */
  @Override
  protected T doSwitch(int classifierID, EObject theEObject)
  {
    switch (classifierID)
    {
      case RulesPackage.RULE_MODEL:
      {
        RuleModel ruleModel = (RuleModel)theEObject;
        T result = caseRuleModel(ruleModel);
        if (result == null) result = defaultCase(theEObject);
        return result;
      }
      case RulesPackage.IMPORT:
      {
        Import import_ = (Import)theEObject;
        T result = caseImport(import_);
        if (result == null) result = defaultCase(theEObject);
        return result;
      }
      case RulesPackage.RULE:
      {
        Rule rule = (Rule)theEObject;
        T result = caseRule(rule);
        if (result == null) result = defaultCase(theEObject);
        return result;
      }
      case RulesPackage.EVENT_TRIGGER:
      {
        EventTrigger eventTrigger = (EventTrigger)theEObject;
        T result = caseEventTrigger(eventTrigger);
        if (result == null) result = defaultCase(theEObject);
        return result;
      }
      case RulesPackage.COMMAND_EVENT_TRIGGER:
      {
        CommandEventTrigger commandEventTrigger = (CommandEventTrigger)theEObject;
        T result = caseCommandEventTrigger(commandEventTrigger);
        if (result == null) result = caseEventTrigger(commandEventTrigger);
        if (result == null) result = defaultCase(theEObject);
        return result;
      }
      case RulesPackage.UPDATE_EVENT_TRIGGER:
      {
        UpdateEventTrigger updateEventTrigger = (UpdateEventTrigger)theEObject;
        T result = caseUpdateEventTrigger(updateEventTrigger);
        if (result == null) result = caseEventTrigger(updateEventTrigger);
        if (result == null) result = defaultCase(theEObject);
        return result;
      }
      case RulesPackage.CHANGED_EVENT_TRIGGER:
      {
        ChangedEventTrigger changedEventTrigger = (ChangedEventTrigger)theEObject;
        T result = caseChangedEventTrigger(changedEventTrigger);
        if (result == null) result = caseEventTrigger(changedEventTrigger);
        if (result == null) result = defaultCase(theEObject);
        return result;
      }
      case RulesPackage.TIMER_TRIGGER:
      {
        TimerTrigger timerTrigger = (TimerTrigger)theEObject;
        T result = caseTimerTrigger(timerTrigger);
        if (result == null) result = caseEventTrigger(timerTrigger);
        if (result == null) result = defaultCase(theEObject);
        return result;
      }
      case RulesPackage.SYSTEM_TRIGGER:
      {
        SystemTrigger systemTrigger = (SystemTrigger)theEObject;
        T result = caseSystemTrigger(systemTrigger);
        if (result == null) result = caseEventTrigger(systemTrigger);
        if (result == null) result = defaultCase(theEObject);
        return result;
      }
      case RulesPackage.SYSTEM_ON_STARTUP_TRIGGER:
      {
        SystemOnStartupTrigger systemOnStartupTrigger = (SystemOnStartupTrigger)theEObject;
        T result = caseSystemOnStartupTrigger(systemOnStartupTrigger);
        if (result == null) result = caseSystemTrigger(systemOnStartupTrigger);
        if (result == null) result = caseEventTrigger(systemOnStartupTrigger);
        if (result == null) result = defaultCase(theEObject);
        return result;
      }
      case RulesPackage.SYSTEM_ON_SHUTDOWN_TRIGGER:
      {
        SystemOnShutdownTrigger systemOnShutdownTrigger = (SystemOnShutdownTrigger)theEObject;
        T result = caseSystemOnShutdownTrigger(systemOnShutdownTrigger);
        if (result == null) result = caseSystemTrigger(systemOnShutdownTrigger);
        if (result == null) result = caseEventTrigger(systemOnShutdownTrigger);
        if (result == null) result = defaultCase(theEObject);
        return result;
      }
      default: return defaultCase(theEObject);
    }
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Rule Model</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null;
   * returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Rule Model</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseRuleModel(RuleModel object)
  {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Import</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null;
   * returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Import</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseImport(Import object)
  {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Rule</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null;
   * returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Rule</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseRule(Rule object)
  {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Event Trigger</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null;
   * returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Event Trigger</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseEventTrigger(EventTrigger object)
  {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Command Event Trigger</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null;
   * returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Command Event Trigger</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseCommandEventTrigger(CommandEventTrigger object)
  {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Update Event Trigger</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null;
   * returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Update Event Trigger</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseUpdateEventTrigger(UpdateEventTrigger object)
  {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Changed Event Trigger</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null;
   * returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Changed Event Trigger</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseChangedEventTrigger(ChangedEventTrigger object)
  {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Timer Trigger</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null;
   * returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Timer Trigger</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseTimerTrigger(TimerTrigger object)
  {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>System Trigger</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null;
   * returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>System Trigger</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseSystemTrigger(SystemTrigger object)
  {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>System On Startup Trigger</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null;
   * returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>System On Startup Trigger</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseSystemOnStartupTrigger(SystemOnStartupTrigger object)
  {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>System On Shutdown Trigger</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null;
   * returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>System On Shutdown Trigger</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseSystemOnShutdownTrigger(SystemOnShutdownTrigger object)
  {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>EObject</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null;
   * returning a non-null result will terminate the switch, but this is the last case anyway.
   * <!-- end-user-doc -->
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>EObject</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject)
   * @generated
   */
  @Override
  public T defaultCase(EObject object)
  {
    return null;
  }

} //RulesSwitch
