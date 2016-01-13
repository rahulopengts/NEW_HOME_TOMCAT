/**
 */
package org.openhab.model.rule.rules;

import org.eclipse.emf.ecore.EAttribute;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.EPackage;
import org.eclipse.emf.ecore.EReference;

/**
 * <!-- begin-user-doc -->
 * The <b>Package</b> for the model.
 * It contains accessors for the meta objects to represent
 * <ul>
 *   <li>each class,</li>
 *   <li>each feature of each class,</li>
 *   <li>each enum,</li>
 *   <li>and each data type</li>
 * </ul>
 * <!-- end-user-doc -->
 * @see org.openhab.model.rule.rules.RulesFactory
 * @model kind="package"
 * @generated
 */
public interface RulesPackage extends EPackage
{
  /**
   * The package name.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  String eNAME = "rules";

  /**
   * The package namespace URI.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  String eNS_URI = "http://www.openhab.org/model/rule/Rules";

  /**
   * The package namespace name.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  String eNS_PREFIX = "rules";

  /**
   * The singleton instance of the package.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  RulesPackage eINSTANCE = org.openhab.model.rule.rules.impl.RulesPackageImpl.init();

  /**
   * The meta object id for the '{@link org.openhab.model.rule.rules.impl.RuleModelImpl <em>Rule Model</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see org.openhab.model.rule.rules.impl.RuleModelImpl
   * @see org.openhab.model.rule.rules.impl.RulesPackageImpl#getRuleModel()
   * @generated
   */
  int RULE_MODEL = 0;

  /**
   * The feature id for the '<em><b>Imports</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int RULE_MODEL__IMPORTS = 0;

  /**
   * The feature id for the '<em><b>Variables</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int RULE_MODEL__VARIABLES = 1;

  /**
   * The feature id for the '<em><b>Rules</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int RULE_MODEL__RULES = 2;

  /**
   * The number of structural features of the '<em>Rule Model</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int RULE_MODEL_FEATURE_COUNT = 3;

  /**
   * The meta object id for the '{@link org.openhab.model.rule.rules.impl.ImportImpl <em>Import</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see org.openhab.model.rule.rules.impl.ImportImpl
   * @see org.openhab.model.rule.rules.impl.RulesPackageImpl#getImport()
   * @generated
   */
  int IMPORT = 1;

  /**
   * The feature id for the '<em><b>Imported Namespace</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int IMPORT__IMPORTED_NAMESPACE = 0;

  /**
   * The number of structural features of the '<em>Import</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int IMPORT_FEATURE_COUNT = 1;

  /**
   * The meta object id for the '{@link org.openhab.model.rule.rules.impl.RuleImpl <em>Rule</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see org.openhab.model.rule.rules.impl.RuleImpl
   * @see org.openhab.model.rule.rules.impl.RulesPackageImpl#getRule()
   * @generated
   */
  int RULE = 2;

  /**
   * The feature id for the '<em><b>Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int RULE__NAME = 0;

  /**
   * The feature id for the '<em><b>Eventtrigger</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int RULE__EVENTTRIGGER = 1;

  /**
   * The feature id for the '<em><b>Script</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int RULE__SCRIPT = 2;

  /**
   * The number of structural features of the '<em>Rule</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int RULE_FEATURE_COUNT = 3;

  /**
   * The meta object id for the '{@link org.openhab.model.rule.rules.impl.EventTriggerImpl <em>Event Trigger</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see org.openhab.model.rule.rules.impl.EventTriggerImpl
   * @see org.openhab.model.rule.rules.impl.RulesPackageImpl#getEventTrigger()
   * @generated
   */
  int EVENT_TRIGGER = 3;

  /**
   * The number of structural features of the '<em>Event Trigger</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int EVENT_TRIGGER_FEATURE_COUNT = 0;

  /**
   * The meta object id for the '{@link org.openhab.model.rule.rules.impl.CommandEventTriggerImpl <em>Command Event Trigger</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see org.openhab.model.rule.rules.impl.CommandEventTriggerImpl
   * @see org.openhab.model.rule.rules.impl.RulesPackageImpl#getCommandEventTrigger()
   * @generated
   */
  int COMMAND_EVENT_TRIGGER = 4;

  /**
   * The feature id for the '<em><b>Item</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int COMMAND_EVENT_TRIGGER__ITEM = EVENT_TRIGGER_FEATURE_COUNT + 0;

  /**
   * The feature id for the '<em><b>Command</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int COMMAND_EVENT_TRIGGER__COMMAND = EVENT_TRIGGER_FEATURE_COUNT + 1;

  /**
   * The number of structural features of the '<em>Command Event Trigger</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int COMMAND_EVENT_TRIGGER_FEATURE_COUNT = EVENT_TRIGGER_FEATURE_COUNT + 2;

  /**
   * The meta object id for the '{@link org.openhab.model.rule.rules.impl.UpdateEventTriggerImpl <em>Update Event Trigger</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see org.openhab.model.rule.rules.impl.UpdateEventTriggerImpl
   * @see org.openhab.model.rule.rules.impl.RulesPackageImpl#getUpdateEventTrigger()
   * @generated
   */
  int UPDATE_EVENT_TRIGGER = 5;

  /**
   * The feature id for the '<em><b>Item</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int UPDATE_EVENT_TRIGGER__ITEM = EVENT_TRIGGER_FEATURE_COUNT + 0;

  /**
   * The feature id for the '<em><b>State</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int UPDATE_EVENT_TRIGGER__STATE = EVENT_TRIGGER_FEATURE_COUNT + 1;

  /**
   * The number of structural features of the '<em>Update Event Trigger</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int UPDATE_EVENT_TRIGGER_FEATURE_COUNT = EVENT_TRIGGER_FEATURE_COUNT + 2;

  /**
   * The meta object id for the '{@link org.openhab.model.rule.rules.impl.ChangedEventTriggerImpl <em>Changed Event Trigger</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see org.openhab.model.rule.rules.impl.ChangedEventTriggerImpl
   * @see org.openhab.model.rule.rules.impl.RulesPackageImpl#getChangedEventTrigger()
   * @generated
   */
  int CHANGED_EVENT_TRIGGER = 6;

  /**
   * The feature id for the '<em><b>Item</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int CHANGED_EVENT_TRIGGER__ITEM = EVENT_TRIGGER_FEATURE_COUNT + 0;

  /**
   * The feature id for the '<em><b>Old State</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int CHANGED_EVENT_TRIGGER__OLD_STATE = EVENT_TRIGGER_FEATURE_COUNT + 1;

  /**
   * The feature id for the '<em><b>New State</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int CHANGED_EVENT_TRIGGER__NEW_STATE = EVENT_TRIGGER_FEATURE_COUNT + 2;

  /**
   * The number of structural features of the '<em>Changed Event Trigger</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int CHANGED_EVENT_TRIGGER_FEATURE_COUNT = EVENT_TRIGGER_FEATURE_COUNT + 3;

  /**
   * The meta object id for the '{@link org.openhab.model.rule.rules.impl.TimerTriggerImpl <em>Timer Trigger</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see org.openhab.model.rule.rules.impl.TimerTriggerImpl
   * @see org.openhab.model.rule.rules.impl.RulesPackageImpl#getTimerTrigger()
   * @generated
   */
  int TIMER_TRIGGER = 7;

  /**
   * The feature id for the '<em><b>Cron</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int TIMER_TRIGGER__CRON = EVENT_TRIGGER_FEATURE_COUNT + 0;

  /**
   * The feature id for the '<em><b>Time</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int TIMER_TRIGGER__TIME = EVENT_TRIGGER_FEATURE_COUNT + 1;

  /**
   * The number of structural features of the '<em>Timer Trigger</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int TIMER_TRIGGER_FEATURE_COUNT = EVENT_TRIGGER_FEATURE_COUNT + 2;

  /**
   * The meta object id for the '{@link org.openhab.model.rule.rules.impl.SystemTriggerImpl <em>System Trigger</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see org.openhab.model.rule.rules.impl.SystemTriggerImpl
   * @see org.openhab.model.rule.rules.impl.RulesPackageImpl#getSystemTrigger()
   * @generated
   */
  int SYSTEM_TRIGGER = 8;

  /**
   * The number of structural features of the '<em>System Trigger</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int SYSTEM_TRIGGER_FEATURE_COUNT = EVENT_TRIGGER_FEATURE_COUNT + 0;

  /**
   * The meta object id for the '{@link org.openhab.model.rule.rules.impl.SystemOnStartupTriggerImpl <em>System On Startup Trigger</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see org.openhab.model.rule.rules.impl.SystemOnStartupTriggerImpl
   * @see org.openhab.model.rule.rules.impl.RulesPackageImpl#getSystemOnStartupTrigger()
   * @generated
   */
  int SYSTEM_ON_STARTUP_TRIGGER = 9;

  /**
   * The number of structural features of the '<em>System On Startup Trigger</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int SYSTEM_ON_STARTUP_TRIGGER_FEATURE_COUNT = SYSTEM_TRIGGER_FEATURE_COUNT + 0;

  /**
   * The meta object id for the '{@link org.openhab.model.rule.rules.impl.SystemOnShutdownTriggerImpl <em>System On Shutdown Trigger</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see org.openhab.model.rule.rules.impl.SystemOnShutdownTriggerImpl
   * @see org.openhab.model.rule.rules.impl.RulesPackageImpl#getSystemOnShutdownTrigger()
   * @generated
   */
  int SYSTEM_ON_SHUTDOWN_TRIGGER = 10;

  /**
   * The number of structural features of the '<em>System On Shutdown Trigger</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int SYSTEM_ON_SHUTDOWN_TRIGGER_FEATURE_COUNT = SYSTEM_TRIGGER_FEATURE_COUNT + 0;


  /**
   * Returns the meta object for class '{@link org.openhab.model.rule.rules.RuleModel <em>Rule Model</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for class '<em>Rule Model</em>'.
   * @see org.openhab.model.rule.rules.RuleModel
   * @generated
   */
  EClass getRuleModel();

  /**
   * Returns the meta object for the containment reference list '{@link org.openhab.model.rule.rules.RuleModel#getImports <em>Imports</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for the containment reference list '<em>Imports</em>'.
   * @see org.openhab.model.rule.rules.RuleModel#getImports()
   * @see #getRuleModel()
   * @generated
   */
  EReference getRuleModel_Imports();

  /**
   * Returns the meta object for the containment reference list '{@link org.openhab.model.rule.rules.RuleModel#getVariables <em>Variables</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for the containment reference list '<em>Variables</em>'.
   * @see org.openhab.model.rule.rules.RuleModel#getVariables()
   * @see #getRuleModel()
   * @generated
   */
  EReference getRuleModel_Variables();

  /**
   * Returns the meta object for the containment reference list '{@link org.openhab.model.rule.rules.RuleModel#getRules <em>Rules</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for the containment reference list '<em>Rules</em>'.
   * @see org.openhab.model.rule.rules.RuleModel#getRules()
   * @see #getRuleModel()
   * @generated
   */
  EReference getRuleModel_Rules();

  /**
   * Returns the meta object for class '{@link org.openhab.model.rule.rules.Import <em>Import</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for class '<em>Import</em>'.
   * @see org.openhab.model.rule.rules.Import
   * @generated
   */
  EClass getImport();

  /**
   * Returns the meta object for the attribute '{@link org.openhab.model.rule.rules.Import#getImportedNamespace <em>Imported Namespace</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for the attribute '<em>Imported Namespace</em>'.
   * @see org.openhab.model.rule.rules.Import#getImportedNamespace()
   * @see #getImport()
   * @generated
   */
  EAttribute getImport_ImportedNamespace();

  /**
   * Returns the meta object for class '{@link org.openhab.model.rule.rules.Rule <em>Rule</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for class '<em>Rule</em>'.
   * @see org.openhab.model.rule.rules.Rule
   * @generated
   */
  EClass getRule();

  /**
   * Returns the meta object for the attribute '{@link org.openhab.model.rule.rules.Rule#getName <em>Name</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for the attribute '<em>Name</em>'.
   * @see org.openhab.model.rule.rules.Rule#getName()
   * @see #getRule()
   * @generated
   */
  EAttribute getRule_Name();

  /**
   * Returns the meta object for the containment reference list '{@link org.openhab.model.rule.rules.Rule#getEventtrigger <em>Eventtrigger</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for the containment reference list '<em>Eventtrigger</em>'.
   * @see org.openhab.model.rule.rules.Rule#getEventtrigger()
   * @see #getRule()
   * @generated
   */
  EReference getRule_Eventtrigger();

  /**
   * Returns the meta object for the containment reference '{@link org.openhab.model.rule.rules.Rule#getScript <em>Script</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for the containment reference '<em>Script</em>'.
   * @see org.openhab.model.rule.rules.Rule#getScript()
   * @see #getRule()
   * @generated
   */
  EReference getRule_Script();

  /**
   * Returns the meta object for class '{@link org.openhab.model.rule.rules.EventTrigger <em>Event Trigger</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for class '<em>Event Trigger</em>'.
   * @see org.openhab.model.rule.rules.EventTrigger
   * @generated
   */
  EClass getEventTrigger();

  /**
   * Returns the meta object for class '{@link org.openhab.model.rule.rules.CommandEventTrigger <em>Command Event Trigger</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for class '<em>Command Event Trigger</em>'.
   * @see org.openhab.model.rule.rules.CommandEventTrigger
   * @generated
   */
  EClass getCommandEventTrigger();

  /**
   * Returns the meta object for the attribute '{@link org.openhab.model.rule.rules.CommandEventTrigger#getItem <em>Item</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for the attribute '<em>Item</em>'.
   * @see org.openhab.model.rule.rules.CommandEventTrigger#getItem()
   * @see #getCommandEventTrigger()
   * @generated
   */
  EAttribute getCommandEventTrigger_Item();

  /**
   * Returns the meta object for the attribute '{@link org.openhab.model.rule.rules.CommandEventTrigger#getCommand <em>Command</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for the attribute '<em>Command</em>'.
   * @see org.openhab.model.rule.rules.CommandEventTrigger#getCommand()
   * @see #getCommandEventTrigger()
   * @generated
   */
  EAttribute getCommandEventTrigger_Command();

  /**
   * Returns the meta object for class '{@link org.openhab.model.rule.rules.UpdateEventTrigger <em>Update Event Trigger</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for class '<em>Update Event Trigger</em>'.
   * @see org.openhab.model.rule.rules.UpdateEventTrigger
   * @generated
   */
  EClass getUpdateEventTrigger();

  /**
   * Returns the meta object for the attribute '{@link org.openhab.model.rule.rules.UpdateEventTrigger#getItem <em>Item</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for the attribute '<em>Item</em>'.
   * @see org.openhab.model.rule.rules.UpdateEventTrigger#getItem()
   * @see #getUpdateEventTrigger()
   * @generated
   */
  EAttribute getUpdateEventTrigger_Item();

  /**
   * Returns the meta object for the attribute '{@link org.openhab.model.rule.rules.UpdateEventTrigger#getState <em>State</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for the attribute '<em>State</em>'.
   * @see org.openhab.model.rule.rules.UpdateEventTrigger#getState()
   * @see #getUpdateEventTrigger()
   * @generated
   */
  EAttribute getUpdateEventTrigger_State();

  /**
   * Returns the meta object for class '{@link org.openhab.model.rule.rules.ChangedEventTrigger <em>Changed Event Trigger</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for class '<em>Changed Event Trigger</em>'.
   * @see org.openhab.model.rule.rules.ChangedEventTrigger
   * @generated
   */
  EClass getChangedEventTrigger();

  /**
   * Returns the meta object for the attribute '{@link org.openhab.model.rule.rules.ChangedEventTrigger#getItem <em>Item</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for the attribute '<em>Item</em>'.
   * @see org.openhab.model.rule.rules.ChangedEventTrigger#getItem()
   * @see #getChangedEventTrigger()
   * @generated
   */
  EAttribute getChangedEventTrigger_Item();

  /**
   * Returns the meta object for the attribute '{@link org.openhab.model.rule.rules.ChangedEventTrigger#getOldState <em>Old State</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for the attribute '<em>Old State</em>'.
   * @see org.openhab.model.rule.rules.ChangedEventTrigger#getOldState()
   * @see #getChangedEventTrigger()
   * @generated
   */
  EAttribute getChangedEventTrigger_OldState();

  /**
   * Returns the meta object for the attribute '{@link org.openhab.model.rule.rules.ChangedEventTrigger#getNewState <em>New State</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for the attribute '<em>New State</em>'.
   * @see org.openhab.model.rule.rules.ChangedEventTrigger#getNewState()
   * @see #getChangedEventTrigger()
   * @generated
   */
  EAttribute getChangedEventTrigger_NewState();

  /**
   * Returns the meta object for class '{@link org.openhab.model.rule.rules.TimerTrigger <em>Timer Trigger</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for class '<em>Timer Trigger</em>'.
   * @see org.openhab.model.rule.rules.TimerTrigger
   * @generated
   */
  EClass getTimerTrigger();

  /**
   * Returns the meta object for the attribute '{@link org.openhab.model.rule.rules.TimerTrigger#getCron <em>Cron</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for the attribute '<em>Cron</em>'.
   * @see org.openhab.model.rule.rules.TimerTrigger#getCron()
   * @see #getTimerTrigger()
   * @generated
   */
  EAttribute getTimerTrigger_Cron();

  /**
   * Returns the meta object for the attribute '{@link org.openhab.model.rule.rules.TimerTrigger#getTime <em>Time</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for the attribute '<em>Time</em>'.
   * @see org.openhab.model.rule.rules.TimerTrigger#getTime()
   * @see #getTimerTrigger()
   * @generated
   */
  EAttribute getTimerTrigger_Time();

  /**
   * Returns the meta object for class '{@link org.openhab.model.rule.rules.SystemTrigger <em>System Trigger</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for class '<em>System Trigger</em>'.
   * @see org.openhab.model.rule.rules.SystemTrigger
   * @generated
   */
  EClass getSystemTrigger();

  /**
   * Returns the meta object for class '{@link org.openhab.model.rule.rules.SystemOnStartupTrigger <em>System On Startup Trigger</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for class '<em>System On Startup Trigger</em>'.
   * @see org.openhab.model.rule.rules.SystemOnStartupTrigger
   * @generated
   */
  EClass getSystemOnStartupTrigger();

  /**
   * Returns the meta object for class '{@link org.openhab.model.rule.rules.SystemOnShutdownTrigger <em>System On Shutdown Trigger</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for class '<em>System On Shutdown Trigger</em>'.
   * @see org.openhab.model.rule.rules.SystemOnShutdownTrigger
   * @generated
   */
  EClass getSystemOnShutdownTrigger();

  /**
   * Returns the factory that creates the instances of the model.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the factory that creates the instances of the model.
   * @generated
   */
  RulesFactory getRulesFactory();

  /**
   * <!-- begin-user-doc -->
   * Defines literals for the meta objects that represent
   * <ul>
   *   <li>each class,</li>
   *   <li>each feature of each class,</li>
   *   <li>each enum,</li>
   *   <li>and each data type</li>
   * </ul>
   * <!-- end-user-doc -->
   * @generated
   */
  interface Literals
  {
    /**
     * The meta object literal for the '{@link org.openhab.model.rule.rules.impl.RuleModelImpl <em>Rule Model</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.openhab.model.rule.rules.impl.RuleModelImpl
     * @see org.openhab.model.rule.rules.impl.RulesPackageImpl#getRuleModel()
     * @generated
     */
    EClass RULE_MODEL = eINSTANCE.getRuleModel();

    /**
     * The meta object literal for the '<em><b>Imports</b></em>' containment reference list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    EReference RULE_MODEL__IMPORTS = eINSTANCE.getRuleModel_Imports();

    /**
     * The meta object literal for the '<em><b>Variables</b></em>' containment reference list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    EReference RULE_MODEL__VARIABLES = eINSTANCE.getRuleModel_Variables();

    /**
     * The meta object literal for the '<em><b>Rules</b></em>' containment reference list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    EReference RULE_MODEL__RULES = eINSTANCE.getRuleModel_Rules();

    /**
     * The meta object literal for the '{@link org.openhab.model.rule.rules.impl.ImportImpl <em>Import</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.openhab.model.rule.rules.impl.ImportImpl
     * @see org.openhab.model.rule.rules.impl.RulesPackageImpl#getImport()
     * @generated
     */
    EClass IMPORT = eINSTANCE.getImport();

    /**
     * The meta object literal for the '<em><b>Imported Namespace</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    EAttribute IMPORT__IMPORTED_NAMESPACE = eINSTANCE.getImport_ImportedNamespace();

    /**
     * The meta object literal for the '{@link org.openhab.model.rule.rules.impl.RuleImpl <em>Rule</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.openhab.model.rule.rules.impl.RuleImpl
     * @see org.openhab.model.rule.rules.impl.RulesPackageImpl#getRule()
     * @generated
     */
    EClass RULE = eINSTANCE.getRule();

    /**
     * The meta object literal for the '<em><b>Name</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    EAttribute RULE__NAME = eINSTANCE.getRule_Name();

    /**
     * The meta object literal for the '<em><b>Eventtrigger</b></em>' containment reference list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    EReference RULE__EVENTTRIGGER = eINSTANCE.getRule_Eventtrigger();

    /**
     * The meta object literal for the '<em><b>Script</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    EReference RULE__SCRIPT = eINSTANCE.getRule_Script();

    /**
     * The meta object literal for the '{@link org.openhab.model.rule.rules.impl.EventTriggerImpl <em>Event Trigger</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.openhab.model.rule.rules.impl.EventTriggerImpl
     * @see org.openhab.model.rule.rules.impl.RulesPackageImpl#getEventTrigger()
     * @generated
     */
    EClass EVENT_TRIGGER = eINSTANCE.getEventTrigger();

    /**
     * The meta object literal for the '{@link org.openhab.model.rule.rules.impl.CommandEventTriggerImpl <em>Command Event Trigger</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.openhab.model.rule.rules.impl.CommandEventTriggerImpl
     * @see org.openhab.model.rule.rules.impl.RulesPackageImpl#getCommandEventTrigger()
     * @generated
     */
    EClass COMMAND_EVENT_TRIGGER = eINSTANCE.getCommandEventTrigger();

    /**
     * The meta object literal for the '<em><b>Item</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    EAttribute COMMAND_EVENT_TRIGGER__ITEM = eINSTANCE.getCommandEventTrigger_Item();

    /**
     * The meta object literal for the '<em><b>Command</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    EAttribute COMMAND_EVENT_TRIGGER__COMMAND = eINSTANCE.getCommandEventTrigger_Command();

    /**
     * The meta object literal for the '{@link org.openhab.model.rule.rules.impl.UpdateEventTriggerImpl <em>Update Event Trigger</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.openhab.model.rule.rules.impl.UpdateEventTriggerImpl
     * @see org.openhab.model.rule.rules.impl.RulesPackageImpl#getUpdateEventTrigger()
     * @generated
     */
    EClass UPDATE_EVENT_TRIGGER = eINSTANCE.getUpdateEventTrigger();

    /**
     * The meta object literal for the '<em><b>Item</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    EAttribute UPDATE_EVENT_TRIGGER__ITEM = eINSTANCE.getUpdateEventTrigger_Item();

    /**
     * The meta object literal for the '<em><b>State</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    EAttribute UPDATE_EVENT_TRIGGER__STATE = eINSTANCE.getUpdateEventTrigger_State();

    /**
     * The meta object literal for the '{@link org.openhab.model.rule.rules.impl.ChangedEventTriggerImpl <em>Changed Event Trigger</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.openhab.model.rule.rules.impl.ChangedEventTriggerImpl
     * @see org.openhab.model.rule.rules.impl.RulesPackageImpl#getChangedEventTrigger()
     * @generated
     */
    EClass CHANGED_EVENT_TRIGGER = eINSTANCE.getChangedEventTrigger();

    /**
     * The meta object literal for the '<em><b>Item</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    EAttribute CHANGED_EVENT_TRIGGER__ITEM = eINSTANCE.getChangedEventTrigger_Item();

    /**
     * The meta object literal for the '<em><b>Old State</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    EAttribute CHANGED_EVENT_TRIGGER__OLD_STATE = eINSTANCE.getChangedEventTrigger_OldState();

    /**
     * The meta object literal for the '<em><b>New State</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    EAttribute CHANGED_EVENT_TRIGGER__NEW_STATE = eINSTANCE.getChangedEventTrigger_NewState();

    /**
     * The meta object literal for the '{@link org.openhab.model.rule.rules.impl.TimerTriggerImpl <em>Timer Trigger</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.openhab.model.rule.rules.impl.TimerTriggerImpl
     * @see org.openhab.model.rule.rules.impl.RulesPackageImpl#getTimerTrigger()
     * @generated
     */
    EClass TIMER_TRIGGER = eINSTANCE.getTimerTrigger();

    /**
     * The meta object literal for the '<em><b>Cron</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    EAttribute TIMER_TRIGGER__CRON = eINSTANCE.getTimerTrigger_Cron();

    /**
     * The meta object literal for the '<em><b>Time</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    EAttribute TIMER_TRIGGER__TIME = eINSTANCE.getTimerTrigger_Time();

    /**
     * The meta object literal for the '{@link org.openhab.model.rule.rules.impl.SystemTriggerImpl <em>System Trigger</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.openhab.model.rule.rules.impl.SystemTriggerImpl
     * @see org.openhab.model.rule.rules.impl.RulesPackageImpl#getSystemTrigger()
     * @generated
     */
    EClass SYSTEM_TRIGGER = eINSTANCE.getSystemTrigger();

    /**
     * The meta object literal for the '{@link org.openhab.model.rule.rules.impl.SystemOnStartupTriggerImpl <em>System On Startup Trigger</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.openhab.model.rule.rules.impl.SystemOnStartupTriggerImpl
     * @see org.openhab.model.rule.rules.impl.RulesPackageImpl#getSystemOnStartupTrigger()
     * @generated
     */
    EClass SYSTEM_ON_STARTUP_TRIGGER = eINSTANCE.getSystemOnStartupTrigger();

    /**
     * The meta object literal for the '{@link org.openhab.model.rule.rules.impl.SystemOnShutdownTriggerImpl <em>System On Shutdown Trigger</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.openhab.model.rule.rules.impl.SystemOnShutdownTriggerImpl
     * @see org.openhab.model.rule.rules.impl.RulesPackageImpl#getSystemOnShutdownTrigger()
     * @generated
     */
    EClass SYSTEM_ON_SHUTDOWN_TRIGGER = eINSTANCE.getSystemOnShutdownTrigger();

  }

} //RulesPackage
