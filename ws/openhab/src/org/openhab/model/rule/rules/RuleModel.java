/**
 */
package org.openhab.model.rule.rules;

import org.eclipse.emf.common.util.EList;

import org.eclipse.emf.ecore.EObject;

import org.eclipse.xtext.xbase.XExpression;

/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Rule Model</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link org.openhab.model.rule.rules.RuleModel#getImports <em>Imports</em>}</li>
 *   <li>{@link org.openhab.model.rule.rules.RuleModel#getVariables <em>Variables</em>}</li>
 *   <li>{@link org.openhab.model.rule.rules.RuleModel#getRules <em>Rules</em>}</li>
 * </ul>
 * </p>
 *
 * @see org.openhab.model.rule.rules.RulesPackage#getRuleModel()
 * @model
 * @generated
 */
public interface RuleModel extends EObject
{
  /**
   * Returns the value of the '<em><b>Imports</b></em>' containment reference list.
   * The list contents are of type {@link org.openhab.model.rule.rules.Import}.
   * <!-- begin-user-doc -->
   * <p>
   * If the meaning of the '<em>Imports</em>' containment reference list isn't clear,
   * there really should be more of a description here...
   * </p>
   * <!-- end-user-doc -->
   * @return the value of the '<em>Imports</em>' containment reference list.
   * @see org.openhab.model.rule.rules.RulesPackage#getRuleModel_Imports()
   * @model containment="true"
   * @generated
   */
  EList<Import> getImports();

  /**
   * Returns the value of the '<em><b>Variables</b></em>' containment reference list.
   * The list contents are of type {@link org.eclipse.xtext.xbase.XExpression}.
   * <!-- begin-user-doc -->
   * <p>
   * If the meaning of the '<em>Variables</em>' containment reference list isn't clear,
   * there really should be more of a description here...
   * </p>
   * <!-- end-user-doc -->
   * @return the value of the '<em>Variables</em>' containment reference list.
   * @see org.openhab.model.rule.rules.RulesPackage#getRuleModel_Variables()
   * @model containment="true"
   * @generated
   */
  EList<XExpression> getVariables();

  /**
   * Returns the value of the '<em><b>Rules</b></em>' containment reference list.
   * The list contents are of type {@link org.openhab.model.rule.rules.Rule}.
   * <!-- begin-user-doc -->
   * <p>
   * If the meaning of the '<em>Rules</em>' containment reference list isn't clear,
   * there really should be more of a description here...
   * </p>
   * <!-- end-user-doc -->
   * @return the value of the '<em>Rules</em>' containment reference list.
   * @see org.openhab.model.rule.rules.RulesPackage#getRuleModel_Rules()
   * @model containment="true"
   * @generated
   */
  EList<Rule> getRules();

} // RuleModel
