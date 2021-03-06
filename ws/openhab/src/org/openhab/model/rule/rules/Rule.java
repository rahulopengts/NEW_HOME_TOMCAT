/**
 */
package org.openhab.model.rule.rules;

import org.eclipse.emf.common.util.EList;

import org.eclipse.emf.ecore.EObject;

import org.eclipse.xtext.xbase.XBlockExpression;

/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Rule</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link org.openhab.model.rule.rules.Rule#getName <em>Name</em>}</li>
 *   <li>{@link org.openhab.model.rule.rules.Rule#getEventtrigger <em>Eventtrigger</em>}</li>
 *   <li>{@link org.openhab.model.rule.rules.Rule#getScript <em>Script</em>}</li>
 * </ul>
 * </p>
 *
 * @see org.openhab.model.rule.rules.RulesPackage#getRule()
 * @model
 * @generated
 */
public interface Rule extends EObject
{
  /**
   * Returns the value of the '<em><b>Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <p>
   * If the meaning of the '<em>Name</em>' attribute isn't clear,
   * there really should be more of a description here...
   * </p>
   * <!-- end-user-doc -->
   * @return the value of the '<em>Name</em>' attribute.
   * @see #setName(String)
   * @see org.openhab.model.rule.rules.RulesPackage#getRule_Name()
   * @model
   * @generated
   */
  String getName();

  /**
   * Sets the value of the '{@link org.openhab.model.rule.rules.Rule#getName <em>Name</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @param value the new value of the '<em>Name</em>' attribute.
   * @see #getName()
   * @generated
   */
  void setName(String value);

  /**
   * Returns the value of the '<em><b>Eventtrigger</b></em>' containment reference list.
   * The list contents are of type {@link org.openhab.model.rule.rules.EventTrigger}.
   * <!-- begin-user-doc -->
   * <p>
   * If the meaning of the '<em>Eventtrigger</em>' containment reference list isn't clear,
   * there really should be more of a description here...
   * </p>
   * <!-- end-user-doc -->
   * @return the value of the '<em>Eventtrigger</em>' containment reference list.
   * @see org.openhab.model.rule.rules.RulesPackage#getRule_Eventtrigger()
   * @model containment="true"
   * @generated
   */
  EList<EventTrigger> getEventtrigger();

  /**
   * Returns the value of the '<em><b>Script</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <p>
   * If the meaning of the '<em>Script</em>' containment reference isn't clear,
   * there really should be more of a description here...
   * </p>
   * <!-- end-user-doc -->
   * @return the value of the '<em>Script</em>' containment reference.
   * @see #setScript(XBlockExpression)
   * @see org.openhab.model.rule.rules.RulesPackage#getRule_Script()
   * @model containment="true"
   * @generated
   */
  XBlockExpression getScript();

  /**
   * Sets the value of the '{@link org.openhab.model.rule.rules.Rule#getScript <em>Script</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @param value the new value of the '<em>Script</em>' containment reference.
   * @see #getScript()
   * @generated
   */
  void setScript(XBlockExpression value);

} // Rule
