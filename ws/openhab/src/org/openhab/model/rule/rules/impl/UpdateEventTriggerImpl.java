/**
 */
package org.openhab.model.rule.rules.impl;

import org.eclipse.emf.common.notify.Notification;

import org.eclipse.emf.ecore.EClass;

import org.eclipse.emf.ecore.impl.ENotificationImpl;

import org.openhab.model.rule.rules.RulesPackage;
import org.openhab.model.rule.rules.UpdateEventTrigger;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Update Event Trigger</b></em>'.
 * <!-- end-user-doc -->
 * <p>
 * The following features are implemented:
 * <ul>
 *   <li>{@link org.openhab.model.rule.rules.impl.UpdateEventTriggerImpl#getItem <em>Item</em>}</li>
 *   <li>{@link org.openhab.model.rule.rules.impl.UpdateEventTriggerImpl#getState <em>State</em>}</li>
 * </ul>
 * </p>
 *
 * @generated
 */
public class UpdateEventTriggerImpl extends EventTriggerImpl implements UpdateEventTrigger
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
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  protected UpdateEventTriggerImpl()
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
    return RulesPackage.Literals.UPDATE_EVENT_TRIGGER;
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
      eNotify(new ENotificationImpl(this, Notification.SET, RulesPackage.UPDATE_EVENT_TRIGGER__ITEM, oldItem, item));
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
      eNotify(new ENotificationImpl(this, Notification.SET, RulesPackage.UPDATE_EVENT_TRIGGER__STATE, oldState, state));
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
      case RulesPackage.UPDATE_EVENT_TRIGGER__ITEM:
        return getItem();
      case RulesPackage.UPDATE_EVENT_TRIGGER__STATE:
        return getState();
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
      case RulesPackage.UPDATE_EVENT_TRIGGER__ITEM:
        setItem((String)newValue);
        return;
      case RulesPackage.UPDATE_EVENT_TRIGGER__STATE:
        setState((String)newValue);
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
      case RulesPackage.UPDATE_EVENT_TRIGGER__ITEM:
        setItem(ITEM_EDEFAULT);
        return;
      case RulesPackage.UPDATE_EVENT_TRIGGER__STATE:
        setState(STATE_EDEFAULT);
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
      case RulesPackage.UPDATE_EVENT_TRIGGER__ITEM:
        return ITEM_EDEFAULT == null ? item != null : !ITEM_EDEFAULT.equals(item);
      case RulesPackage.UPDATE_EVENT_TRIGGER__STATE:
        return STATE_EDEFAULT == null ? state != null : !STATE_EDEFAULT.equals(state);
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
    result.append(", state: ");
    result.append(state);
    result.append(')');
    return result.toString();
  }

} //UpdateEventTriggerImpl
