/**
 */
package org.openhab.model.rule.rules;


/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Changed Event Trigger</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link org.openhab.model.rule.rules.ChangedEventTrigger#getItem <em>Item</em>}</li>
 *   <li>{@link org.openhab.model.rule.rules.ChangedEventTrigger#getOldState <em>Old State</em>}</li>
 *   <li>{@link org.openhab.model.rule.rules.ChangedEventTrigger#getNewState <em>New State</em>}</li>
 * </ul>
 * </p>
 *
 * @see org.openhab.model.rule.rules.RulesPackage#getChangedEventTrigger()
 * @model
 * @generated
 */
public interface ChangedEventTrigger extends EventTrigger
{
  /**
   * Returns the value of the '<em><b>Item</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <p>
   * If the meaning of the '<em>Item</em>' attribute isn't clear,
   * there really should be more of a description here...
   * </p>
   * <!-- end-user-doc -->
   * @return the value of the '<em>Item</em>' attribute.
   * @see #setItem(String)
   * @see org.openhab.model.rule.rules.RulesPackage#getChangedEventTrigger_Item()
   * @model
   * @generated
   */
  String getItem();

  /**
   * Sets the value of the '{@link org.openhab.model.rule.rules.ChangedEventTrigger#getItem <em>Item</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @param value the new value of the '<em>Item</em>' attribute.
   * @see #getItem()
   * @generated
   */
  void setItem(String value);

  /**
   * Returns the value of the '<em><b>Old State</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <p>
   * If the meaning of the '<em>Old State</em>' attribute isn't clear,
   * there really should be more of a description here...
   * </p>
   * <!-- end-user-doc -->
   * @return the value of the '<em>Old State</em>' attribute.
   * @see #setOldState(String)
   * @see org.openhab.model.rule.rules.RulesPackage#getChangedEventTrigger_OldState()
   * @model
   * @generated
   */
  String getOldState();

  /**
   * Sets the value of the '{@link org.openhab.model.rule.rules.ChangedEventTrigger#getOldState <em>Old State</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @param value the new value of the '<em>Old State</em>' attribute.
   * @see #getOldState()
   * @generated
   */
  void setOldState(String value);

  /**
   * Returns the value of the '<em><b>New State</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <p>
   * If the meaning of the '<em>New State</em>' attribute isn't clear,
   * there really should be more of a description here...
   * </p>
   * <!-- end-user-doc -->
   * @return the value of the '<em>New State</em>' attribute.
   * @see #setNewState(String)
   * @see org.openhab.model.rule.rules.RulesPackage#getChangedEventTrigger_NewState()
   * @model
   * @generated
   */
  String getNewState();

  /**
   * Sets the value of the '{@link org.openhab.model.rule.rules.ChangedEventTrigger#getNewState <em>New State</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @param value the new value of the '<em>New State</em>' attribute.
   * @see #getNewState()
   * @generated
   */
  void setNewState(String value);

} // ChangedEventTrigger
