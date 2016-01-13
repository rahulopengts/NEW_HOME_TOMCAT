/**
 */
package org.openhab.model.sitemap.impl;

import org.eclipse.emf.common.notify.Notification;

import org.eclipse.emf.ecore.EClass;

import org.eclipse.emf.ecore.impl.ENotificationImpl;

import org.openhab.model.sitemap.SitemapPackage;
import org.openhab.model.sitemap.Slider;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Slider</b></em>'.
 * <!-- end-user-doc -->
 * <p>
 * The following features are implemented:
 * <ul>
 *   <li>{@link org.openhab.model.sitemap.impl.SliderImpl#getFrequency <em>Frequency</em>}</li>
 *   <li>{@link org.openhab.model.sitemap.impl.SliderImpl#isSwitchEnabled <em>Switch Enabled</em>}</li>
 * </ul>
 * </p>
 *
 * @generated
 */
public class SliderImpl extends NonLinkableWidgetImpl implements Slider
{
  /**
   * The default value of the '{@link #getFrequency() <em>Frequency</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see #getFrequency()
   * @generated
   * @ordered
   */
  protected static final int FREQUENCY_EDEFAULT = 0;

  /**
   * The cached value of the '{@link #getFrequency() <em>Frequency</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see #getFrequency()
   * @generated
   * @ordered
   */
  protected int frequency = FREQUENCY_EDEFAULT;

  /**
   * The default value of the '{@link #isSwitchEnabled() <em>Switch Enabled</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see #isSwitchEnabled()
   * @generated
   * @ordered
   */
  protected static final boolean SWITCH_ENABLED_EDEFAULT = false;

  /**
   * The cached value of the '{@link #isSwitchEnabled() <em>Switch Enabled</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see #isSwitchEnabled()
   * @generated
   * @ordered
   */
  protected boolean switchEnabled = SWITCH_ENABLED_EDEFAULT;

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  protected SliderImpl()
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
    return SitemapPackage.Literals.SLIDER;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public int getFrequency()
  {
    return frequency;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public void setFrequency(int newFrequency)
  {
    int oldFrequency = frequency;
    frequency = newFrequency;
    if (eNotificationRequired())
      eNotify(new ENotificationImpl(this, Notification.SET, SitemapPackage.SLIDER__FREQUENCY, oldFrequency, frequency));
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public boolean isSwitchEnabled()
  {
    return switchEnabled;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public void setSwitchEnabled(boolean newSwitchEnabled)
  {
    boolean oldSwitchEnabled = switchEnabled;
    switchEnabled = newSwitchEnabled;
    if (eNotificationRequired())
      eNotify(new ENotificationImpl(this, Notification.SET, SitemapPackage.SLIDER__SWITCH_ENABLED, oldSwitchEnabled, switchEnabled));
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
      case SitemapPackage.SLIDER__FREQUENCY:
        return getFrequency();
      case SitemapPackage.SLIDER__SWITCH_ENABLED:
        return isSwitchEnabled();
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
      case SitemapPackage.SLIDER__FREQUENCY:
        setFrequency((Integer)newValue);
        return;
      case SitemapPackage.SLIDER__SWITCH_ENABLED:
        setSwitchEnabled((Boolean)newValue);
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
      case SitemapPackage.SLIDER__FREQUENCY:
        setFrequency(FREQUENCY_EDEFAULT);
        return;
      case SitemapPackage.SLIDER__SWITCH_ENABLED:
        setSwitchEnabled(SWITCH_ENABLED_EDEFAULT);
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
      case SitemapPackage.SLIDER__FREQUENCY:
        return frequency != FREQUENCY_EDEFAULT;
      case SitemapPackage.SLIDER__SWITCH_ENABLED:
        return switchEnabled != SWITCH_ENABLED_EDEFAULT;
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
    result.append(" (frequency: ");
    result.append(frequency);
    result.append(", switchEnabled: ");
    result.append(switchEnabled);
    result.append(')');
    return result.toString();
  }

} //SliderImpl
