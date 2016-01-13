/**
 */
package org.openhab.model.rule.rules;


/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Command Event Trigger</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link org.openhab.model.rule.rules.CommandEventTrigger#getItem <em>Item</em>}</li>
 *   <li>{@link org.openhab.model.rule.rules.CommandEventTrigger#getCommand <em>Command</em>}</li>
 * </ul>
 * </p>
 *
 * @see org.openhab.model.rule.rules.RulesPackage#getCommandEventTrigger()
 * @model
 * @generated
 */
public interface CommandEventTrigger extends EventTrigger
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
   * @see org.openhab.model.rule.rules.RulesPackage#getCommandEventTrigger_Item()
   * @model
   * @generated
   */
  String getItem();

  /**
   * Sets the value of the '{@link org.openhab.model.rule.rules.CommandEventTrigger#getItem <em>Item</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @param value the new value of the '<em>Item</em>' attribute.
   * @see #getItem()
   * @generated
   */
  void setItem(String value);

  /**
   * Returns the value of the '<em><b>Command</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <p>
   * If the meaning of the '<em>Command</em>' attribute isn't clear,
   * there really should be more of a description here...
   * </p>
   * <!-- end-user-doc -->
   * @return the value of the '<em>Command</em>' attribute.
   * @see #setCommand(String)
   * @see org.openhab.model.rule.rules.RulesPackage#getCommandEventTrigger_Command()
   * @model
   * @generated
   */
  String getCommand();

  /**
   * Sets the value of the '{@link org.openhab.model.rule.rules.CommandEventTrigger#getCommand <em>Command</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @param value the new value of the '<em>Command</em>' attribute.
   * @see #getCommand()
   * @generated
   */
  void setCommand(String value);

} // CommandEventTrigger
