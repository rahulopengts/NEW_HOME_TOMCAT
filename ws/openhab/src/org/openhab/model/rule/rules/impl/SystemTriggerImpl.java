/**
 */
package org.openhab.model.rule.rules.impl;

import org.eclipse.emf.ecore.EClass;

import org.openhab.model.rule.rules.RulesPackage;
import org.openhab.model.rule.rules.SystemTrigger;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>System Trigger</b></em>'.
 * <!-- end-user-doc -->
 * <p>
 * </p>
 *
 * @generated
 */
public class SystemTriggerImpl extends EventTriggerImpl implements SystemTrigger
{
  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  protected SystemTriggerImpl()
  {
    super();
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  @Override
  protected EClass eStaticClass()
  {
    return RulesPackage.Literals.SYSTEM_TRIGGER;
  }

} //SystemTriggerImpl
