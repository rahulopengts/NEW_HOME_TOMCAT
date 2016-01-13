/**
 */
package org.openhab.model.sitemap.impl;

import org.eclipse.emf.common.notify.Notification;

import org.eclipse.emf.ecore.EClass;

import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.impl.MinimalEObjectImpl;

import org.openhab.model.sitemap.ColorArray;
import org.openhab.model.sitemap.SitemapPackage;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Color Array</b></em>'.
 * <!-- end-user-doc -->
 * <p>
 * The following features are implemented:
 * <ul>
 *   <li>{@link org.openhab.model.sitemap.impl.ColorArrayImpl#getItem <em>Item</em>}</li>
 *   <li>{@link org.openhab.model.sitemap.impl.ColorArrayImpl#getCondition <em>Condition</em>}</li>
 *   <li>{@link org.openhab.model.sitemap.impl.ColorArrayImpl#getSign <em>Sign</em>}</li>
 *   <li>{@link org.openhab.model.sitemap.impl.ColorArrayImpl#getState <em>State</em>}</li>
 *   <li>{@link org.openhab.model.sitemap.impl.ColorArrayImpl#getArg <em>Arg</em>}</li>
 * </ul>
 * </p>
 *
 * @generated
 */
public class ColorArrayImpl extends MinimalEObjectImpl.Container implements ColorArray
{
  /**
   * The default value of the '{@link #getItem() <em>Item</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see #getItem()
   * @generated
   * @ordered
   */
  protected static final String ITEM_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getItem() <em>Item</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see #getItem()
   * @generated
   * @ordered
   */
  protected String item = ITEM_EDEFAULT;

  /**
   * The default value of the '{@link #getCondition() <em>Condition</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see #getCondition()
   * @generated
   * @ordered
   */
  protected static final String CONDITION_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getCondition() <em>Condition</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see #getCondition()
   * @generated
   * @ordered
   */
  protected String condition = CONDITION_EDEFAULT;

  /**
   * The default value of the '{@link #getSign() <em>Sign</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see #getSign()
   * @generated
   * @ordered
   */
  protected static final String SIGN_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getSign() <em>Sign</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see #getSign()
   * @generated
   * @ordered
   */
  protected String sign = SIGN_EDEFAULT;

  /**
   * The default value of the '{@link #getState() <em>State</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see #getState()
   * @generated
   * @ordered
   */
  protected static final String STATE_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getState() <em>State</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see #getState()
   * @generated
   * @ordered
   */
  protected String state = STATE_EDEFAULT;

  /**
   * The default value of the '{@link #getArg() <em>Arg</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see #getArg()
   * @generated
   * @ordered
   */
  protected static final String ARG_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getArg() <em>Arg</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see #getArg()
   * @generated
   * @ordered
   */
  protected String arg = ARG_EDEFAULT;

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  protected ColorArrayImpl()
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
    return SitemapPackage.Literals.COLOR_ARRAY;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public String getItem()
  {
    return item;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public void setItem(String newItem)
  {
    String oldItem = item;
    item = newItem;
    if (eNotificationRequired())
      eNotify(new ENotificationImpl(this, Notification.SET, SitemapPackage.COLOR_ARRAY__ITEM, oldItem, item));
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public String getCondition()
  {
    return condition;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public void setCondition(String newCondition)
  {
    String oldCondition = condition;
    condition = newCondition;
    if (eNotificationRequired())
      eNotify(new ENotificationImpl(this, Notification.SET, SitemapPackage.COLOR_ARRAY__CONDITION, oldCondition, condition));
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public String getSign()
  {
    return sign;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public void setSign(String newSign)
  {
    String oldSign = sign;
    sign = newSign;
    if (eNotificationRequired())
      eNotify(new ENotificationImpl(this, Notification.SET, SitemapPackage.COLOR_ARRAY__SIGN, oldSign, sign));
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public String getState()
  {
    return state;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public void setState(String newState)
  {
    String oldState = state;
    state = newState;
    if (eNotificationRequired())
      eNotify(new ENotificationImpl(this, Notification.SET, SitemapPackage.COLOR_ARRAY__STATE, oldState, state));
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public String getArg()
  {
    return arg;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public void setArg(String newArg)
  {
    String oldArg = arg;
    arg = newArg;
    if (eNotificationRequired())
      eNotify(new ENotificationImpl(this, Notification.SET, SitemapPackage.COLOR_ARRAY__ARG, oldArg, arg));
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
      case SitemapPackage.COLOR_ARRAY__ITEM:
        return getItem();
      case SitemapPackage.COLOR_ARRAY__CONDITION:
        return getCondition();
      case SitemapPackage.COLOR_ARRAY__SIGN:
        return getSign();
      case SitemapPackage.COLOR_ARRAY__STATE:
        return getState();
      case SitemapPackage.COLOR_ARRAY__ARG:
        return getArg();
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
      case SitemapPackage.COLOR_ARRAY__ITEM:
        setItem((String)newValue);
        return;
      case SitemapPackage.COLOR_ARRAY__CONDITION:
        setCondition((String)newValue);
        return;
      case SitemapPackage.COLOR_ARRAY__SIGN:
        setSign((String)newValue);
        return;
      case SitemapPackage.COLOR_ARRAY__STATE:
        setState((String)newValue);
        return;
      case SitemapPackage.COLOR_ARRAY__ARG:
        setArg((String)newValue);
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
      case SitemapPackage.COLOR_ARRAY__ITEM:
        setItem(ITEM_EDEFAULT);
        return;
      case SitemapPackage.COLOR_ARRAY__CONDITION:
        setCondition(CONDITION_EDEFAULT);
        return;
      case SitemapPackage.COLOR_ARRAY__SIGN:
        setSign(SIGN_EDEFAULT);
        return;
      case SitemapPackage.COLOR_ARRAY__STATE:
        setState(STATE_EDEFAULT);
        return;
      case SitemapPackage.COLOR_ARRAY__ARG:
        setArg(ARG_EDEFAULT);
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
      case SitemapPackage.COLOR_ARRAY__ITEM:
        return ITEM_EDEFAULT == null ? item != null : !ITEM_EDEFAULT.equals(item);
      case SitemapPackage.COLOR_ARRAY__CONDITION:
        return CONDITION_EDEFAULT == null ? condition != null : !CONDITION_EDEFAULT.equals(condition);
      case SitemapPackage.COLOR_ARRAY__SIGN:
        return SIGN_EDEFAULT == null ? sign != null : !SIGN_EDEFAULT.equals(sign);
      case SitemapPackage.COLOR_ARRAY__STATE:
        return STATE_EDEFAULT == null ? state != null : !STATE_EDEFAULT.equals(state);
      case SitemapPackage.COLOR_ARRAY__ARG:
        return ARG_EDEFAULT == null ? arg != null : !ARG_EDEFAULT.equals(arg);
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
    result.append(" (item: ");
    result.append(item);
    result.append(", condition: ");
    result.append(condition);
    result.append(", sign: ");
    result.append(sign);
    result.append(", state: ");
    result.append(state);
    result.append(", arg: ");
    result.append(arg);
    result.append(')');
    return result.toString();
  }

} //ColorArrayImpl
