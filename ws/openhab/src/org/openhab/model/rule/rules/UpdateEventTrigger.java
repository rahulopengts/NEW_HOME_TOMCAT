/**
 */
package org.openhab.model.rule.rules;


/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Update Event Trigger</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link org.openhab.model.rule.rules.UpdateEventTrigger#getItem <em>Item</em>}</li>
 *   <li>{@link org.openhab.model.rule.rules.UpdateEventTrigger#getState <em>State</em>}</li>
 * </ul>
 * </p>
 *
 * @see org.openhab.model.rule.rules.RulesPackage#getUpdateEventTrigger()
 * @model
 * @generated
 */
public interface UpdateEventTrigger extends EventTrigger
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
   * @see org.openhab.model.rule.rules.RulesPackage#getUpdateEventTrigger_Item()
   * @model
   * @generated
   */
  String getItem();

  /**
   * Sets the value of the '{@link org.openhab.model.rule.rules.UpdateEventTrigger#getItem <em>Item</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @param value the new value of the '<em>Item</em>' attribute.
   * @see #getItem()
   * @generated
   */
  void setItem(String value);

  /**
   * Returns the value of the '<em><b>State</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <p>
   * If the meaning of the '<em>State</em>' attribute isn't clear,
   * there really should be more of a description here...
   * </p>
   * <!-- end-user-doc -->
   * @return the value of the '<em>State</em>' attribute.
   * @see #setState(String)
   * @see org.openhab.model.rule.rules.RulesPackage#getUpdateEventTrigger_State()
   * @model
   * @generated
   */
  String getState();

  /**
   * Sets the value of the '{@link org.openhab.model.rule.rules.UpdateEventTrigger#getState <em>State</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @param value the new value of the '<em>State</em>' attribute.
   * @see #getState()
   * @generated
   */
  void setState(String value);

} // UpdateEventTrigger
