/**
 */
package org.openhab.model.sitemap.impl;

import org.eclipse.emf.common.notify.Notification;

import org.eclipse.emf.ecore.EClass;

import org.eclipse.emf.ecore.impl.ENotificationImpl;

import org.openhab.model.sitemap.Chart;
import org.openhab.model.sitemap.SitemapPackage;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Chart</b></em>'.
 * <!-- end-user-doc -->
 * <p>
 * The following features are implemented:
 * <ul>
 *   <li>{@link org.openhab.model.sitemap.impl.ChartImpl#getService <em>Service</em>}</li>
 *   <li>{@link org.openhab.model.sitemap.impl.ChartImpl#getRefresh <em>Refresh</em>}</li>
 *   <li>{@link org.openhab.model.sitemap.impl.ChartImpl#getPeriod <em>Period</em>}</li>
 * </ul>
 * </p>
 *
 * @generated
 */
public class ChartImpl extends NonLinkableWidgetImpl implements Chart
{
  /**
   * The default value of the '{@link #getService() <em>Service</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see #getService()
   * @generated
   * @ordered
   */
  protected static final String SERVICE_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getService() <em>Service</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see #getService()
   * @generated
   * @ordered
   */
  protected String service = SERVICE_EDEFAULT;

  /**
   * The default value of the '{@link #getRefresh() <em>Refresh</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see #getRefresh()
   * @generated
   * @ordered
   */
  protected static final int REFRESH_EDEFAULT = 0;

  /**
   * The cached value of the '{@link #getRefresh() <em>Refresh</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see #getRefresh()
   * @generated
   * @ordered
   */
  protected int refresh = REFRESH_EDEFAULT;

  /**
   * The default value of the '{@link #getPeriod() <em>Period</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see #getPeriod()
   * @generated
   * @ordered
   */
  protected static final String PERIOD_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getPeriod() <em>Period</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see #getPeriod()
   * @generated
   * @ordered
   */
  protected String period = PERIOD_EDEFAULT;

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  protected ChartImpl()
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
    return SitemapPackage.Literals.CHART;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public String getService()
  {
    return service;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public void setService(String newService)
  {
    String oldService = service;
    service = newService;
    if (eNotificationRequired())
      eNotify(new ENotificationImpl(this, Notification.SET, SitemapPackage.CHART__SERVICE, oldService, service));
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public int getRefresh()
  {
    return refresh;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public void setRefresh(int newRefresh)
  {
    int oldRefresh = refresh;
    refresh = newRefresh;
    if (eNotificationRequired())
      eNotify(new ENotificationImpl(this, Notification.SET, SitemapPackage.CHART__REFRESH, oldRefresh, refresh));
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public String getPeriod()
  {
    return period;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public void setPeriod(String newPeriod)
  {
    String oldPeriod = period;
    period = newPeriod;
    if (eNotificationRequired())
      eNotify(new ENotificationImpl(this, Notification.SET, SitemapPackage.CHART__PERIOD, oldPeriod, period));
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
      case SitemapPackage.CHART__SERVICE:
        return getService();
      case SitemapPackage.CHART__REFRESH:
        return getRefresh();
      case SitemapPackage.CHART__PERIOD:
        return getPeriod();
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
      case SitemapPackage.CHART__SERVICE:
        setService((String)newValue);
        return;
      case SitemapPackage.CHART__REFRESH:
        setRefresh((Integer)newValue);
        return;
      case SitemapPackage.CHART__PERIOD:
        setPeriod((String)newValue);
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
      case SitemapPackage.CHART__SERVICE:
        setService(SERVICE_EDEFAULT);
        return;
      case SitemapPackage.CHART__REFRESH:
        setRefresh(REFRESH_EDEFAULT);
        return;
      case SitemapPackage.CHART__PERIOD:
        setPeriod(PERIOD_EDEFAULT);
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
      case SitemapPackage.CHART__SERVICE:
        return SERVICE_EDEFAULT == null ? service != null : !SERVICE_EDEFAULT.equals(service);
      case SitemapPackage.CHART__REFRESH:
        return refresh != REFRESH_EDEFAULT;
      case SitemapPackage.CHART__PERIOD:
        return PERIOD_EDEFAULT == null ? period != null : !PERIOD_EDEFAULT.equals(period);
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
    result.append(" (service: ");
    result.append(service);
    result.append(", refresh: ");
    result.append(refresh);
    result.append(", period: ");
    result.append(period);
    result.append(')');
    return result.toString();
  }

} //ChartImpl
