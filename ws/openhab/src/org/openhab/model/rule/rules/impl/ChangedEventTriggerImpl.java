/**
 */
package org.openhab.model.rule.rules.impl;

import org.eclipse.emf.common.notify.Notification;

import org.eclipse.emf.ecore.EClass;

import org.eclipse.emf.ecore.impl.ENotificationImpl;

import org.openhab.model.rule.rules.ChangedEventTrigger;
import org.openhab.model.rule.rules.RulesPackage;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Changed Event Trigger</b></em>'.
 * <!-- end-user-doc -->
 * <p>
 * The following features are implemented:
 * <ul>
 *   <li>{@link org.openhab.model.rule.rules.impl.ChangedEventTriggerImpl#getItem <em>Item</em>}</li>
 *   <li>{@link org.openhab.model.rule.rules.impl.ChangedEventTriggerImpl#getOldState <em>Old State</em>}</li>
 *   <li>{@link org.openhab.model.rule.rules.impl.ChangedEventTriggerImpl#getNewState <em>New State</em>}</li>
 * </ul>
 * </p>
 *
 * @generated
 */
public class ChangedEventTriggerImpl extends EventTriggerImpl implements ChangedEventTrigger
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
   * The default value of the '{@link #getOldState() <em>Old State</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see #getOldState()
   * @generated
   * @ordered
   */
  protected static final String OLD_STATE_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getOldState() <em>Old State</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see #getOldState()
   * @generated
   * @ordered
   */
  protected String oldState = OLD_STATE_EDEFAULT;

  /**
   * The default value of the '{@link #getNewState() <em>New State</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see #getNewState()
   * @generated
   * @ordered
   */
  protected static final String NEW_STATE_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getNewState() <em>New State</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see #getNewState()
   * @generated
   * @ordered
   */
  protected String newState = NEW_STATE_EDEFAULT;

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  protected ChangedEventTriggerImpl()
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
    return RulesPackage.Literals.CHANGED_EVENT_TRIGGER;
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
      eNotify(new ENotificationImpl(this, Notification.SET, RulesPackage.CHANGED_EVENT_TRIGGER__ITEM, oldItem, item));
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public String getOldState()
  {
    return oldState;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public void setOldState(String newOldState)
  {
    String oldOldState = oldState;
    oldState = newOldState;
    if (eNotificationRequired())
      eNotify(new ENotificationImpl(this, Notification.SET, RulesPackage.CHANGED_EVENT_TRIGGER__OLD_STATE, oldOldState, oldState));
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public String getNewState()
  {
    return newState;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public void setNewState(String newNewState)
  {
    String oldNewState = newState;
    newState = newNewState;
    if (eNotificationRequired())
      eNotify(new ENotificationImpl(this, Notification.SET, RulesPackage.CHANGED_EVENT_TRIGGER__NEW_STATE, oldNewState, newState));
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
      case RulesPackage.CHANGED_EVENT_TRIGGER__ITEM:
        return getItem();
      case RulesPackage.CHANGED_EVENT_TRIGGER__OLD_STATE:
        return getOldState();
      case RulesPackage.CHANGED_EVENT_TRIGGER__NEW_STATE:
        return getNewState();
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
      case RulesPackage.CHANGED_EVENT_TRIGGER__ITEM:
        setItem((String)newValue);
        return;
      case RulesPackage.CHANGED_EVENT_TRIGGER__OLD_STATE:
        setOldState((String)newValue);
        return;
      case RulesPackage.CHANGED_EVENT_TRIGGER__NEW_STATE:
        setNewState((String)newValue);
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
      case RulesPackage.CHANGED_EVENT_TRIGGER__ITEM:
        setItem(ITEM_EDEFAULT);
        return;
      case RulesPackage.CHANGED_EVENT_TRIGGER__OLD_STATE:
        setOldState(OLD_STATE_EDEFAULT);
        return;
      case RulesPackage.CHANGED_EVENT_TRIGGER__NEW_STATE:
        setNewState(NEW_STATE_EDEFAULT);
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
      case RulesPackage.CHANGED_EVENT_TRIGGER__ITEM:
        return ITEM_EDEFAULT == null ? item != null : !ITEM_EDEFAULT.equals(item);
      case RulesPackage.CHANGED_EVENT_TRIGGER__OLD_STATE:
        return OLD_STATE_EDEFAULT == null ? oldState != null : !OLD_STATE_EDEFAULT.equals(oldState);
      case RulesPackage.CHANGED_EVENT_TRIGGER__NEW_STATE:
        return NEW_STATE_EDEFAULT == null ? newState != null : !NEW_STATE_EDEFAULT.equals(newState);
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
    result.append(", oldState: ");
    result.append(oldState);
    result.append(", newState: ");
    result.append(newState);
    result.append(')');
    return result.toString();
  }

} //ChangedEventTriggerImpl
