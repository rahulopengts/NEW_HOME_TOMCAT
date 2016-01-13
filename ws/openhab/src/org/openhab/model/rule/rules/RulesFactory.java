/**
 */
package org.openhab.model.rule.rules;

import org.eclipse.emf.ecore.EFactory;

/**
 * <!-- begin-user-doc -->
 * The <b>Factory</b> for the model.
 * It provides a create method for each non-abstract class of the model.
 * <!-- end-user-doc -->
 * @see org.openhab.model.rule.rules.RulesPackage
 * @generated
 */
public interface RulesFactory extends EFactory
{
  /**
   * The singleton instance of the factory.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  RulesFactory eINSTANCE = org.openhab.model.rule.rules.impl.RulesFactoryImpl.init();

  /**
   * Returns a new object of class '<em>Rule Model</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return a new object of class '<em>Rule Model</em>'.
   * @generated
   */
  RuleModel createRuleModel();

  /**
   * Returns a new object of class '<em>Import</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return a new object of class '<em>Import</em>'.
   * @generated
   */
  Import createImport();

  /**
   * Returns a new object of class '<em>Rule</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return a new object of class '<em>Rule</em>'.
   * @generated
   */
  Rule createRule();

  /**
   * Returns a new object of class '<em>Event Trigger</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return a new object of class '<em>Event Trigger</em>'.
   * @generated
   */
  EventTrigger createEventTrigger();

  /**
   * Returns a new object of class '<em>Command Event Trigger</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return a new object of class '<em>Command Event Trigger</em>'.
   * @generated
   */
  CommandEventTrigger createCommandEventTrigger();

  /**
   * Returns a new object of class '<em>Update Event Trigger</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return a new object of class '<em>Update Event Trigger</em>'.
   * @generated
   */
  UpdateEventTrigger createUpdateEventTrigger();

  /**
   * Returns a new object of class '<em>Changed Event Trigger</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return a new object of class '<em>Changed Event Trigger</em>'.
   * @generated
   */
  ChangedEventTrigger createChangedEventTrigger();

  /**
   * Returns a new object of class '<em>Timer Trigger</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return a new object of class '<em>Timer Trigger</em>'.
   * @generated
   */
  TimerTrigger createTimerTrigger();

  /**
   * Returns a new object of class '<em>System Trigger</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return a new object of class '<em>System Trigger</em>'.
   * @generated
   */
  SystemTrigger createSystemTrigger();

  /**
   * Returns a new object of class '<em>System On Startup Trigger</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return a new object of class '<em>System On Startup Trigger</em>'.
   * @generated
   */
  SystemOnStartupTrigger createSystemOnStartupTrigger();

  /**
   * Returns a new object of class '<em>System On Shutdown Trigger</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return a new object of class '<em>System On Shutdown Trigger</em>'.
   * @generated
   */
  SystemOnShutdownTrigger createSystemOnShutdownTrigger();

  /**
   * Returns the package supported by this factory.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the package supported by this factory.
   * @generated
   */
  RulesPackage getRulesPackage();

} //RulesFactory
