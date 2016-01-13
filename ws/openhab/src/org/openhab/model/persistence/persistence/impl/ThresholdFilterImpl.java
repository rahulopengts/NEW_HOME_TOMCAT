/**
 */
package org.openhab.model.persistence.persistence.impl;

import java.math.BigDecimal;

import org.eclipse.emf.common.notify.Notification;

import org.eclipse.emf.ecore.EClass;

import org.eclipse.emf.ecore.impl.ENotificationImpl;

import org.openhab.model.persistence.persistence.PersistencePackage;
import org.openhab.model.persistence.persistence.ThresholdFilter;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Threshold Filter</b></em>'.
 * <!-- end-user-doc -->
 * <p>
 * The following features are implemented:
 * <ul>
 *   <li>{@link org.openhab.model.persistence.persistence.impl.ThresholdFilterImpl#getValue <em>Value</em>}</li>
 *   <li>{@link org.openhab.model.persistence.persistence.impl.ThresholdFilterImpl#isPercent <em>Percent</em>}</li>
 * </ul>
 * </p>
 *
 * @generated
 */
public class ThresholdFilterImpl extends FilterDetailsImpl implements ThresholdFilter
{
  /**
   * The default value of the '{@link #getValue() <em>Value</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see #getValue()
   * @generated
   * @ordered
   */
  protected static final BigDecimal VALUE_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getValue() <em>Value</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see #getValue()
   * @generated
   * @ordered
   */
  protected BigDecimal value = VALUE_EDEFAULT;

  /**
   * The default value of the '{@link #isPercent() <em>Percent</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see #isPercent()
   * @generated
   * @ordered
   */
  protected static final boolean PERCENT_EDEFAULT = false;

  /**
   * The cached value of the '{@link #isPercent() <em>Percent</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see #isPercent()
   * @generated
   * @ordered
   */
  protected boolean percent = PERCENT_EDEFAULT;

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  protected ThresholdFilterImpl()
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
    return PersistencePackage.Literals.THRESHOLD_FILTER;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public BigDecimal getValue()
  {
    return value;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public void setValue(BigDecimal newValue)
  {
    BigDecimal oldValue = value;
    value = newValue;
    if (eNotificationRequired())
      eNotify(new ENotificationImpl(this, Notification.SET, PersistencePackage.THRESHOLD_FILTER__VALUE, oldValue, value));
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public boolean isPercent()
  {
    return percent;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public void setPercent(boolean newPercent)
  {
    boolean oldPercent = percent;
    percent = newPercent;
    if (eNotificationRequired())
      eNotify(new ENotificationImpl(this, Notification.SET, PersistencePackage.THRESHOLD_FILTER__PERCENT, oldPercent, percent));
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  @Override
  public Object eGet(int featureID, boolean resolve, boolean coreType)
  {
    switch (featureID)
    {
      case PersistencePackage.THRESHOLD_FILTER__VALUE:
        return getValue();
      case PersistencePackage.THRESHOLD_FILTER__PERCENT:
        return isPercent();
    }
    return super.eGet(featureID, resolve, coreType);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  @Override
  public void eSet(int featureID, Object newValue)
  {
    switch (featureID)
    {
      case PersistencePackage.THRESHOLD_FILTER__VALUE:
        setValue((BigDecimal)newValue);
        return;
      case PersistencePackage.THRESHOLD_FILTER__PERCENT:
        setPercent((Boolean)newValue);
        return;
    }
    super.eSet(featureID, newValue);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  @Override
  public void eUnset(int featureID)
  {
    switch (featureID)
    {
      case PersistencePackage.THRESHOLD_FILTER__VALUE:
        setValue(VALUE_EDEFAULT);
        return;
      case PersistencePackage.THRESHOLD_FILTER__PERCENT:
        setPercent(PERCENT_EDEFAULT);
        return;
    }
    super.eUnset(featureID);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  @Override
  public boolean eIsSet(int featureID)
  {
    switch (featureID)
    {
      case PersistencePackage.THRESHOLD_FILTER__VALUE:
        return VALUE_EDEFAULT == null ? value != null : !VALUE_EDEFAULT.equals(value);
      case PersistencePackage.THRESHOLD_FILTER__PERCENT:
        return percent != PERCENT_EDEFAULT;
    }
    return super.eIsSet(featureID);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  @Override
  public String toString()
  {
    if (eIsProxy()) return super.toString();

    StringBuffer result = new StringBuffer(super.toString());
    result.append(" (value: ");
    result.append(value);
    result.append(", percent: ");
    result.append(percent);
    result.append(')');
    return result.toString();
  }

} //ThresholdFilterImpl
