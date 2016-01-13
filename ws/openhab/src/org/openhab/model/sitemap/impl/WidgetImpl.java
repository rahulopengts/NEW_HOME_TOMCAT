/**
 */
package org.openhab.model.sitemap.impl;

import java.util.Collection;

import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;

import org.eclipse.emf.common.util.EList;

import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;

import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.impl.MinimalEObjectImpl;

import org.eclipse.emf.ecore.util.EObjectContainmentEList;
import org.eclipse.emf.ecore.util.InternalEList;

import org.openhab.model.sitemap.ColorArray;
import org.openhab.model.sitemap.SitemapPackage;
import org.openhab.model.sitemap.VisibilityRule;
import org.openhab.model.sitemap.Widget;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Widget</b></em>'.
 * <!-- end-user-doc -->
 * <p>
 * The following features are implemented:
 * <ul>
 *   <li>{@link org.openhab.model.sitemap.impl.WidgetImpl#getItem <em>Item</em>}</li>
 *   <li>{@link org.openhab.model.sitemap.impl.WidgetImpl#getLabel <em>Label</em>}</li>
 *   <li>{@link org.openhab.model.sitemap.impl.WidgetImpl#getIcon <em>Icon</em>}</li>
 *   <li>{@link org.openhab.model.sitemap.impl.WidgetImpl#getLabelColor <em>Label Color</em>}</li>
 *   <li>{@link org.openhab.model.sitemap.impl.WidgetImpl#getValueColor <em>Value Color</em>}</li>
 *   <li>{@link org.openhab.model.sitemap.impl.WidgetImpl#getVisibility <em>Visibility</em>}</li>
 * </ul>
 * </p>
 *
 * @generated
 */
public class WidgetImpl extends MinimalEObjectImpl.Container implements Widget
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
   * The default value of the '{@link #getLabel() <em>Label</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see #getLabel()
   * @generated
   * @ordered
   */
  protected static final String LABEL_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getLabel() <em>Label</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see #getLabel()
   * @generated
   * @ordered
   */
  protected String label = LABEL_EDEFAULT;

  /**
   * The default value of the '{@link #getIcon() <em>Icon</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see #getIcon()
   * @generated
   * @ordered
   */
  protected static final String ICON_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getIcon() <em>Icon</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see #getIcon()
   * @generated
   * @ordered
   */
  protected String icon = ICON_EDEFAULT;

  /**
   * The cached value of the '{@link #getLabelColor() <em>Label Color</em>}' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see #getLabelColor()
   * @generated
   * @ordered
   */
  protected EList<ColorArray> labelColor;

  /**
   * The cached value of the '{@link #getValueColor() <em>Value Color</em>}' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see #getValueColor()
   * @generated
   * @ordered
   */
  protected EList<ColorArray> valueColor;

  /**
   * The cached value of the '{@link #getVisibility() <em>Visibility</em>}' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see #getVisibility()
   * @generated
   * @ordered
   */
  protected EList<VisibilityRule> visibility;

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  protected WidgetImpl()
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
    return SitemapPackage.Literals.WIDGET;
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
      eNotify(new ENotificationImpl(this, Notification.SET, SitemapPackage.WIDGET__ITEM, oldItem, item));
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public String getLabel()
  {
    return label;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public void setLabel(String newLabel)
  {
    String oldLabel = label;
    label = newLabel;
    if (eNotificationRequired())
      eNotify(new ENotificationImpl(this, Notification.SET, SitemapPackage.WIDGET__LABEL, oldLabel, label));
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public String getIcon()
  {
    return icon;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public void setIcon(String newIcon)
  {
    String oldIcon = icon;
    icon = newIcon;
    if (eNotificationRequired())
      eNotify(new ENotificationImpl(this, Notification.SET, SitemapPackage.WIDGET__ICON, oldIcon, icon));
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EList<ColorArray> getLabelColor()
  {
    if (labelColor == null)
    {
      labelColor = new EObjectContainmentEList<ColorArray>(ColorArray.class, this, SitemapPackage.WIDGET__LABEL_COLOR);
    }
    return labelColor;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EList<ColorArray> getValueColor()
  {
    if (valueColor == null)
    {
      valueColor = new EObjectContainmentEList<ColorArray>(ColorArray.class, this, SitemapPackage.WIDGET__VALUE_COLOR);
    }
    return valueColor;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EList<VisibilityRule> getVisibility()
  {
    if (visibility == null)
    {
      visibility = new EObjectContainmentEList<VisibilityRule>(VisibilityRule.class, this, SitemapPackage.WIDGET__VISIBILITY);
    }
    return visibility;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  @Override
  public NotificationChain eInverseRemove(InternalEObject otherEnd, int featureID, NotificationChain msgs)
  {
    switch (featureID)
    {
      case SitemapPackage.WIDGET__LABEL_COLOR:
        return ((InternalEList<?>)getLabelColor()).basicRemove(otherEnd, msgs);
      case SitemapPackage.WIDGET__VALUE_COLOR:
        return ((InternalEList<?>)getValueColor()).basicRemove(otherEnd, msgs);
      case SitemapPackage.WIDGET__VISIBILITY:
        return ((InternalEList<?>)getVisibility()).basicRemove(otherEnd, msgs);
    }
    return super.eInverseRemove(otherEnd, featureID, msgs);
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
      case SitemapPackage.WIDGET__ITEM:
        return getItem();
      case SitemapPackage.WIDGET__LABEL:
        return getLabel();
      case SitemapPackage.WIDGET__ICON:
        return getIcon();
      case SitemapPackage.WIDGET__LABEL_COLOR:
        return getLabelColor();
      case SitemapPackage.WIDGET__VALUE_COLOR:
        return getValueColor();
      case SitemapPackage.WIDGET__VISIBILITY:
        return getVisibility();
    }
    return super.eGet(featureID, resolve, coreType);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  @SuppressWarnings("unchecked")
  @Override
  public void eSet(int featureID, Object newValue)
  {
    switch (featureID)
    {
      case SitemapPackage.WIDGET__ITEM:
        setItem((String)newValue);
        return;
      case SitemapPackage.WIDGET__LABEL:
        setLabel((String)newValue);
        return;
      case SitemapPackage.WIDGET__ICON:
        setIcon((String)newValue);
        return;
      case SitemapPackage.WIDGET__LABEL_COLOR:
        getLabelColor().clear();
        getLabelColor().addAll((Collection<? extends ColorArray>)newValue);
        return;
      case SitemapPackage.WIDGET__VALUE_COLOR:
        getValueColor().clear();
        getValueColor().addAll((Collection<? extends ColorArray>)newValue);
        return;
      case SitemapPackage.WIDGET__VISIBILITY:
        getVisibility().clear();
        getVisibility().addAll((Collection<? extends VisibilityRule>)newValue);
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
      case SitemapPackage.WIDGET__ITEM:
        setItem(ITEM_EDEFAULT);
        return;
      case SitemapPackage.WIDGET__LABEL:
        setLabel(LABEL_EDEFAULT);
        return;
      case SitemapPackage.WIDGET__ICON:
        setIcon(ICON_EDEFAULT);
        return;
      case SitemapPackage.WIDGET__LABEL_COLOR:
        getLabelColor().clear();
        return;
      case SitemapPackage.WIDGET__VALUE_COLOR:
        getValueColor().clear();
        return;
      case SitemapPackage.WIDGET__VISIBILITY:
        getVisibility().clear();
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
      case SitemapPackage.WIDGET__ITEM:
        return ITEM_EDEFAULT == null ? item != null : !ITEM_EDEFAULT.equals(item);
      case SitemapPackage.WIDGET__LABEL:
        return LABEL_EDEFAULT == null ? label != null : !LABEL_EDEFAULT.equals(label);
      case SitemapPackage.WIDGET__ICON:
        return ICON_EDEFAULT == null ? icon != null : !ICON_EDEFAULT.equals(icon);
      case SitemapPackage.WIDGET__LABEL_COLOR:
        return labelColor != null && !labelColor.isEmpty();
      case SitemapPackage.WIDGET__VALUE_COLOR:
        return valueColor != null && !valueColor.isEmpty();
      case SitemapPackage.WIDGET__VISIBILITY:
        return visibility != null && !visibility.isEmpty();
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
    result.append(", label: ");
    result.append(label);
    result.append(", icon: ");
    result.append(icon);
    result.append(')');
    return result.toString();
  }

} //WidgetImpl
