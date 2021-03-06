/**
 */
package org.openhab.model.rule.rules;


/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Timer Trigger</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link org.openhab.model.rule.rules.TimerTrigger#getCron <em>Cron</em>}</li>
 *   <li>{@link org.openhab.model.rule.rules.TimerTrigger#getTime <em>Time</em>}</li>
 * </ul>
 * </p>
 *
 * @see org.openhab.model.rule.rules.RulesPackage#getTimerTrigger()
 * @model
 * @generated
 */
public interface TimerTrigger extends EventTrigger
{
  /**
   * Returns the value of the '<em><b>Cron</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <p>
   * If the meaning of the '<em>Cron</em>' attribute isn't clear,
   * there really should be more of a description here...
   * </p>
   * <!-- end-user-doc -->
   * @return the value of the '<em>Cron</em>' attribute.
   * @see #setCron(String)
   * @see org.openhab.model.rule.rules.RulesPackage#getTimerTrigger_Cron()
   * @model
   * @generated
   */
  String getCron();

  /**
   * Sets the value of the '{@link org.openhab.model.rule.rules.TimerTrigger#getCron <em>Cron</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @param value the new value of the '<em>Cron</em>' attribute.
   * @see #getCron()
   * @generated
   */
  void setCron(String value);

  /**
   * Returns the value of the '<em><b>Time</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <p>
   * If the meaning of the '<em>Time</em>' attribute isn't clear,
   * there really should be more of a description here...
   * </p>
   * <!-- end-user-doc -->
   * @return the value of the '<em>Time</em>' attribute.
   * @see #setTime(String)
   * @see org.openhab.model.rule.rules.RulesPackage#getTimerTrigger_Time()
   * @model
   * @generated
   */
  String getTime();

  /**
   * Sets the value of the '{@link org.openhab.model.rule.rules.TimerTrigger#getTime <em>Time</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @param value the new value of the '<em>Time</em>' attribute.
   * @see #getTime()
   * @generated
   */
  void setTime(String value);

} // TimerTrigger
